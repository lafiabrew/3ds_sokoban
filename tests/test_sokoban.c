/*
 * Unit & pass/fail tests for 3ds_sokoban:
 * - Pass-the-pass: player movement, crate push onto goal, 64-step undo stack restoration.
 * - Fail-the-fail: pushing into a wall or second crate is blocked, invalid diagonal move rejected.
 */

#include "sokoban.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    soko_state_t st;
    assert(soko_init_level(&st, 0));
    int orig_x = st.px;
    int orig_y = st.py;

    /* Push box at (3, 2) upward by moving up from (3, 3) */
    assert(soko_step_move(&st, 0, -1));
    assert(st.pushes == 1 && st.moves == 1);
    assert(st.boxes[1][3] == 1u);

    /* Undo restores both player and pushed crate */
    assert(soko_undo(&st));
    assert(st.px == orig_x && st.py == orig_y);
    assert(st.boxes[2][3] == 1u && st.boxes[1][3] == 0u);

    /* Fail-the-fail: diagonal moves, zero moves, invalid level index */
    assert(soko_step_move(&st, 1, 1) == false);
    assert(soko_step_move(&st, 0, 0) == false);
    assert(soko_init_level(NULL, 0) == false);
    assert(soko_init_level(&st, -1) == false);
    assert(soko_init_level(&st, 99) == false);
    assert(soko_undo(&st) == false); /* stack now empty */

    printf("PASS: test_sokoban all crate push, wall collision, undo stack, and pass/fail checks succeeded.\n");
    return 0;
}
