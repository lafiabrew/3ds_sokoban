#include "sokoban.h"

#include <string.h>

static const char *const k_levels[SOKO_LEVELS][SOKO_H] = {
    {
        "##########",
        "#........#",
        "#..$..*..#",
        "#..@..$..#",
        "#.....*..#",
        "#........#",
        "#........#",
        "##########"
    },
    {
        "##########",
        "#..####..#",
        "#..*..$..#",
        "#..#..#..#",
        "#..$..*..#",
        "#....@...#",
        "#........#",
        "##########"
    },
    {
        "##########",
        "#........#",
        "#..*.*.*.#",
        "#..$.$.$.#",
        "#....@...#",
        "#........#",
        "#........#",
        "##########"
    },
    {
        "##########",
        "#...##...#",
        "#.*.$$.*.#",
        "#...@@...#",
        "#........#",
        "#........#",
        "#........#",
        "##########"
    }
};

bool soko_is_solved(const soko_state_t *st) {
    if (st == NULL) return false;
    for (int y = 0; y < SOKO_H; y++) {
        for (int x = 0; x < SOKO_W; x++) {
            if (st->map[y][x] == SOKO_GOAL && !st->boxes[y][x]) {
                return false;
            }
        }
    }
    return true;
}

bool soko_init_level(soko_state_t *st, int level_idx) {
    if (st == NULL || level_idx < 0 || level_idx >= SOKO_LEVELS) {
        return false;
    }
    memset(st, 0, sizeof(*st));
    st->level_idx = level_idx;
    for (int y = 0; y < SOKO_H; y++) {
        const char *row = k_levels[level_idx][y];
        for (int x = 0; x < SOKO_W; x++) {
            char c = row[x];
            if (c == '#') {
                st->map[y][x] = SOKO_WALL;
            } else if (c == '*') {
                st->map[y][x] = SOKO_GOAL;
            } else if (c == '$') {
                st->map[y][x] = SOKO_FLOOR;
                st->boxes[y][x] = 1u;
            } else if (c == '@') {
                st->map[y][x] = SOKO_FLOOR;
                st->px = x;
                st->py = y;
            }
        }
    }
    st->solved = soko_is_solved(st);
    return true;
}

bool soko_step_move(soko_state_t *st, int dx, int dy) {
    if (st == NULL || (dx == 0 && dy == 0) || (dx != 0 && dy != 0)) {
        return false;
    }
    if (dx < -1 || dx > 1 || dy < -1 || dy > 1) return false;

    int nx = st->px + dx;
    int ny = st->py + dy;
    if (nx < 0 || nx >= SOKO_W || ny < 0 || ny >= SOKO_H) return false;
    if (st->map[ny][nx] == SOKO_WALL) return false;

    bool pushed = false;
    if (st->boxes[ny][nx]) {
        int bx = nx + dx;
        int by = ny + dy;
        if (bx < 0 || bx >= SOKO_W || by < 0 || by >= SOKO_H) return false;
        if (st->map[by][bx] == SOKO_WALL || st->boxes[by][bx]) return false;

        if (st->undo_len < SOKO_MAX_UNDO) {
            soko_snapshot_t snap;
            snap.px = (uint8_t)st->px;
            snap.py = (uint8_t)st->py;
            memcpy(snap.boxes, st->boxes, sizeof(st->boxes));
            st->undo_stack[st->undo_len++] = snap;
        }
        st->boxes[ny][nx] = 0;
        st->boxes[by][bx] = 1u;
        pushed = true;
    } else if (st->undo_len < SOKO_MAX_UNDO) {
        soko_snapshot_t snap;
        snap.px = (uint8_t)st->px;
        snap.py = (uint8_t)st->py;
        memcpy(snap.boxes, st->boxes, sizeof(st->boxes));
        st->undo_stack[st->undo_len++] = snap;
    }

    st->px = nx;
    st->py = ny;
    st->moves++;
    if (pushed) st->pushes++;
    st->solved = soko_is_solved(st);
    return true;
}

bool soko_undo(soko_state_t *st) {
    if (st == NULL || st->undo_len <= 0) return false;
    soko_snapshot_t snap = st->undo_stack[--st->undo_len];
    st->px = snap.px;
    st->py = snap.py;
    memcpy(st->boxes, snap.boxes, sizeof(st->boxes));
    st->solved = soko_is_solved(st);
    return true;
}
