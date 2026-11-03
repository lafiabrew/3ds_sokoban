#ifndef SOKOBAN_H
#define SOKOBAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SOKO_W 10
#define SOKO_H 8
#define SOKO_LEVELS 4
#define SOKO_MAX_UNDO 64

typedef enum {
    SOKO_FLOOR = 0,
    SOKO_WALL  = 1,
    SOKO_GOAL  = 2
} soko_tile_t;

typedef struct {
    uint8_t px;
    uint8_t py;
    uint8_t boxes[SOKO_H][SOKO_W];
} soko_snapshot_t;

typedef struct {
    int level_idx;
    soko_tile_t map[SOKO_H][SOKO_W];
    uint8_t boxes[SOKO_H][SOKO_W];
    int px;
    int py;
    uint32_t moves;
    uint32_t pushes;
    soko_snapshot_t undo_stack[SOKO_MAX_UNDO];
    int undo_len;
    bool solved;
} soko_state_t;

bool soko_init_level(soko_state_t *st, int level_idx);
bool soko_step_move(soko_state_t *st, int dx, int dy);
bool soko_undo(soko_state_t *st);
bool soko_is_solved(const soko_state_t *st);

#endif /* SOKOBAN_H */
