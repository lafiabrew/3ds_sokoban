#include "pk_platform.h"
#include "sokoban.h"

#include <stdio.h>
#include <string.h>

typedef struct {
    char sd_root[512];
    soko_state_t st;
    char status_msg[128];
} soko_app_t;

static void app_init(soko_app_t *app, const char *sd_root) {
    memset(app, 0, sizeof(*app));
    snprintf(app->sd_root, sizeof(app->sd_root), "%s",
             sd_root != NULL ? sd_root : pk_platform_default_sd_root());
    soko_init_level(&app->st, 0);
    snprintf(app->status_msg, sizeof(app->status_msg),
             "WAREHOUSE 1/%d | D-PAD:PUSH B:UNDO X:RESET L/R:LEVEL", SOKO_LEVELS);
    pk_log_event(app->sd_root, "sokoban", "INFO", app->status_msg);
}

static void app_step(soko_app_t *app, const pk_input_t *in) {
    if (in->down & PK_BTN_DUP)    soko_step_move(&app->st, 0, -1);
    if (in->down & PK_BTN_DDOWN)  soko_step_move(&app->st, 0,  1);
    if (in->down & PK_BTN_DLEFT)  soko_step_move(&app->st, -1, 0);
    if (in->down & PK_BTN_DRIGHT) soko_step_move(&app->st,  1, 0);
    if (in->down & PK_BTN_B)      soko_undo(&app->st);
    if (in->down & PK_BTN_X)      soko_init_level(&app->st, app->st.level_idx);
    if (in->down & PK_BTN_L)      soko_init_level(&app->st, (app->st.level_idx + SOKO_LEVELS - 1) % SOKO_LEVELS);
    if (in->down & PK_BTN_R)      soko_init_level(&app->st, (app->st.level_idx + 1) % SOKO_LEVELS);

    snprintf(app->status_msg, sizeof(app->status_msg),
             "LEVEL %d/%d | MOVES:%u PUSHES:%u | %s",
             app->st.level_idx + 1, SOKO_LEVELS,
             (unsigned)app->st.moves, (unsigned)app->st.pushes,
             app->st.solved ? "*CLEARED!* (PRESS R)" : "B:UNDO X:RETRY");
}

static void app_render(const soko_app_t *app) {
    pk_render_begin_frame();
    pk_render_begin_target(PK_TARGET_TOP_LEFT, PK_RGBA8(22, 18, 14, 255));
    pk_draw_rect(0, 0, 400, 26, PK_RGBA8(92, 62, 34, 255));
    pk_draw_text(8, 7, 0.48f, PK_RGBA8(255, 225, 110, 255),
                 "SOKOBAN 3DS - CLASSIC WAREHOUSE PUZZLE");

    for (int y = 0; y < SOKO_H; y++) {
        for (int x = 0; x < SOKO_W; x++) {
            float tx = 80.0f + (float)x * 24.0f;
            float ty = 32.0f + (float)y * 22.0f;
            soko_tile_t t = app->st.map[y][x];
            uint32_t col = (t == SOKO_WALL) ? PK_RGBA8(95, 105, 125, 255)
                         : (t == SOKO_GOAL) ? PK_RGBA8(55, 130, 85, 255)
                                            : PK_RGBA8(38, 32, 26, 255);
            pk_draw_rect(tx, ty, 22.0f, 20.0f, col);
            if (app->st.boxes[y][x]) {
                uint32_t bcol = (t == SOKO_GOAL) ? PK_RGBA8(95, 245, 145, 255)
                                                 : PK_RGBA8(235, 165, 65, 255);
                pk_draw_rect(tx + 3.0f, ty + 3.0f, 16.0f, 14.0f, bcol);
            } else if (x == app->st.px && y == app->st.py) {
                pk_draw_rect(tx + 5.0f, ty + 4.0f, 12.0f, 12.0f, PK_RGBA8(95, 210, 255, 255));
            }
        }
    }

    pk_draw_rect(0, 216, 400, 24, PK_RGBA8(54, 38, 22, 255));
    pk_draw_text(8, 222, 0.44f, PK_RGBA8(130, 250, 175, 255), app->status_msg);

    pk_render_begin_target(PK_TARGET_BOTTOM, PK_RGBA8(18, 14, 12, 255));
    pk_draw_rect(16, 20, 288, 200, PK_RGBA8(32, 26, 20, 255));
    pk_draw_text(28, 36, 0.50f, PK_RGBA8(255, 220, 95, 255), "D-PAD: MOVE & PUSH CRATES");
    pk_draw_text(28, 66, 0.48f, PK_RGBA8(135, 235, 255, 255), "B BUTTON: UNDO LAST MOVE (64 STEPS)");
    pk_draw_text(28, 96, 0.48f, PK_RGBA8(255, 165, 110, 255), "X BUTTON: RESTART LEVEL");
    pk_draw_text(28, 126, 0.48f, PK_RGBA8(145, 250, 175, 255), "L / R: PREVIOUS / NEXT WAREHOUSE");
    pk_draw_text(28, 174, 0.44f, PK_RGBA8(190, 180, 170, 255), "PRESS START TO EXIT");
    pk_render_end_frame();
}

int main(int argc, char **argv) {
    const char *sd_root = NULL;
    bool one_shot = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--sd-root") == 0 && i + 1 < argc) sd_root = argv[++i];
        else if (strcmp(argv[i], "--once") == 0) one_shot = true;
    }
    if (!pk_platform_init()) return 1;
    soko_app_t app;
    app_init(&app, sd_root);
    while (pk_platform_main_loop()) {
        pk_input_t in;
        pk_platform_poll_input(&in);
        if (in.down & PK_BTN_START) break;
        app_step(&app, &in);
        app_render(&app);
        if (one_shot) break;
    }
    pk_platform_shutdown();
    return 0;
}
