#ifndef LEZAC_RECOVERED_H
#define LEZAC_RECOVERED_H

#include <stdint.h>

/* Semantic C interfaces, NOT the original segmented Pascal ABI.
 * All buffers/pointers must be valid and mutually non-overlapping.
 * See docs/recovered-routines.md for machine-code evidence and scope. */

/* 1000:0000: count <= 65535; seed is the existing accumulator. */
uint16_t lezac_alternating_checksum(uint16_t seed, const uint8_t *bytes,
                                    uint16_t count);

/* 1000:06ab: {frame,start,end,counter,delay,mode,step} becomes
 * {start,start,end,delay,delay,mode,1}. Animation, not sound state. */
void lezac_animation_init(uint8_t state[7], uint8_t start, uint8_t end,
                          uint8_t delay, uint8_t mode);

/* 1000:5715: modulo-256 addition BEFORE unsigned clamping. */
uint8_t lezac_wrapping_add_clamp(uint8_t value, uint8_t delta, uint8_t limit);

/* 1000:5b86: magnitude <=42 becomes zero; INT16_MIN also becomes zero. */
int16_t lezac_friction_step(int16_t velocity);

/* 1000:2e8d: byte-exact little-endian state; loop_index models DS:2082. */
void lezac_state45_init(uint8_t state[45], uint16_t fill_word,
                        uint16_t *loop_index);

typedef struct lezac_collection {
    uint16_t score;          /* DS:2074, accumulated modulo 65536 */
    uint16_t bonus;          /* DS:2072 */
    uint8_t collected_tile;  /* DS:79a3 */
    uint8_t above_present;   /* DS:661e, 0 or 1 */
} lezac_collection;

/* 1000:5afd: caller resolves the tile and word-map addresses. Both tables
 * contain 12 native-endian words, corresponding to tile IDs 0x67..0x72.
 * above_cell is read before any map update; state->score is an input too. */
void lezac_collect_tile(uint8_t *tile, uint16_t *cell, uint16_t above_cell,
                        const uint16_t score_values[12],
                        const uint16_t bonus_values[12],
                        lezac_collection *state);

#endif
