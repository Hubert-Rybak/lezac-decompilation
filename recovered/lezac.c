/* Hand reconstruction from original LEZAC.EXE instructions, with REA/Ghidra
 * evidence cross-checked against relocated executable bytes. Not generated
 * decompiler output; no claim of original source names or a playable port. */
#include "lezac.h"

uint16_t lezac_alternating_checksum(uint16_t seed, const uint8_t *bytes,
                                    uint16_t count)
{
    uint32_t i;
    for (i = 0; i < count; ++i) {
        if ((i & 1u) == 0)
            seed = (uint16_t)(seed + bytes[i]);
        else
            seed = (uint16_t)(seed - bytes[i]);
    }
    return seed;
}

void lezac_animation_init(uint8_t state[7], uint8_t start, uint8_t end,
                          uint8_t delay, uint8_t mode)
{
    state[0] = start;
    state[1] = start;
    state[2] = end;
    state[4] = delay;
    state[5] = mode;
    state[6] = 1;
    state[3] = delay;
}

uint8_t lezac_wrapping_add_clamp(uint8_t value, uint8_t delta, uint8_t limit)
{
    uint8_t wrapped = (uint8_t)(value + delta);
    return wrapped > limit ? limit : wrapped;
}

int16_t lezac_friction_step(int16_t velocity)
{
    /* C abs(-32768) is not the DOS 16-bit CWD/XOR/SUB sequence:
     * that sequence leaves 0x8000, then signed JGE treats it as negative. */
    if (velocity == INT16_MIN || (velocity > -43 && velocity < 43))
        return 0;
    return (int16_t)(velocity < 0 ? velocity + 42 : velocity - 42);
}

static void store_le16(uint8_t *destination, uint16_t value)
{
    destination[0] = (uint8_t)value;
    destination[1] = (uint8_t)(value >> 8);
}

void lezac_state45_init(uint8_t state[45], uint16_t fill_word,
                        uint16_t *loop_index)
{
    unsigned i;
    for (i = 0; i != 10; ++i) {
        store_le16(state + 4 + 2 * i, fill_word);
        store_le16(state + 24 + 2 * i, fill_word);
    }
    state[44] = 2;
    store_le16(state, 0);
    store_le16(state + 2, 0);
    *loop_index = 9;
}

void lezac_collect_tile(uint8_t *tile, uint16_t *cell, uint16_t above_cell,
                        const uint16_t score_values[12],
                        const uint16_t bonus_values[12],
                        lezac_collection *state)
{
    unsigned index;
    state->collected_tile = 0;
    state->bonus = 0;
    state->above_present = 0;
    /* Signed AL comparisons in the original accept exactly this positive
     * range; high-bit tile IDs are not collectibles. */
    if (*tile < 0x67 || *tile > 0x72)
        return;
    state->collected_tile = *tile;
    index = (unsigned)*tile - 0x67;
    state->bonus = bonus_values[index];
    state->score = (uint16_t)((uint32_t)state->score + score_values[index]);
    state->above_present = (uint8_t)(above_cell != 0 && above_cell < 0x8000);
    if (*cell < 0x8000) {
        *cell = 0;
        *tile = 0;
    } else {
        *tile = 0xFF;
    }
}
