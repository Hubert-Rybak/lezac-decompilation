# Recovered portable C routines

This is a **six-routine behavioral reconstruction**, not recovered original source,
not a rebuilt DOS executable, and not a playable game. The C API deliberately
replaces segmented pointers, nested Pascal stack frames, and selected globals
with ordinary values, buffers, and explicit output state.

## Evidence and executable oracle

The authoritative input is `original/game/LEZAC.EXE` (52,384 bytes), SHA-256:

```text
7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec
```

REA/Ghidra evidence is retained under
`analysis/rea/verified/functions/<linear-address>.json`: `10000.json`,
`106ab.json`, `15715.json`, `15b86.json`, `12e8d.json`, and `15afd.json`.
`16053.json` supplies actor-update/caller context; `196df.json` supplies the
runtime stack check. Addresses below are linear addresses at load segment
`0x1000`; e.g. `0x15715` is `1000:5715`.

Reconstruction used the actual assembly, including stack offsets, operand widths,
conditional branch signedness, and return cleanup. The bytes were independently
disassembled after MZ relocation, rather than trusting inferred decompiler
signatures. Tests do **not** execute a translation of that assembly as their
oracle: Unicorn executes the original binary's load image.

`tests/dos_machine.py`:

- Checks the input hash, reads the 1,904-byte MZ header, loads the full load image
  at `0x10000`, and adds `0x1000` to all 468 relocation words.
- Maps 2 MiB and supplies isolated fixture data with `DS=3000`, `SS=8000`,
  `SP=f000`; the synthetic caller's return IP is `ff00` in segment `1000`.
- Executes every original prologue and its original runtime stack check at
  `1920:04df` / linear `0x196df`. The actual relocated far-call bytes are
  `9a df 04 20 19`; Ghidra can render the equivalent address as `1000:96df`.
- Leaves `DS:1afa=0` and sufficient stack space, so the check follows its normal
  success path. It is **not** stubbed, skipped, or replaced with a return.
- Stops at the sentinel before executing it, checks BP restoration and the
  exact callee-cleaned SP, and requires one stack-check execution per invocation.
- Protects all pages holding the load image against guest writes and checks the
  entire relocated image and on-disk executable hash after the suite.
- Treats an unexpected interrupt as a failure. No interrupt, BIOS, DOS, hardware,
  runtime, or subroutine behavior is supplied by replacement hooks.

Synthetic input data, caller frames, and the normal DOS relocation fixups are the
only setup changes. The instruction-count limit detects a failure to return.

## Original ABI versus C interface

All six entries are near calls. Offsets below are relative to BP **after** the
routine's `PUSH BP; MOV BP,SP`. Each byte parameter still consumes a two-byte
stack slot; only its low byte is read. Far pointers are offset word followed by
segment word. The C interfaces do not promise register/flags equivalence.

| Entry / inclusive body | Original inputs | Cleanup | C interface |
|---|---|---|---|
| `10000..10067` | `+04=count`; `+06/+08=accumulator far pointer`; `+0a/+0c=source far pointer` | `RET 0x0a` | `lezac_alternating_checksum(seed, bytes, count)` returns updated word |
| `106ab..10708` | `+04=mode`; `+06=delay`; `+08=end`; `+0a=start`; `+0c/+0e=state far pointer` | `RET 0x0c` | `lezac_animation_init(state, start, end, delay, mode)` |
| `15715..1573f` | `+04=limit`; `+06=delta`; `+08/+0a=value far pointer` | `RET 0x08` | `lezac_wrapping_add_clamp(value, delta, limit)` returns byte |
| `15b86..15bcb` | `+04=parent frame offset`; input/output word is `SS:[frame-0x0c]` | `RET 0x02` | `lezac_friction_step(velocity)` returns signed word |
| `12e8d..12ef1` | `+04=unused static link`; `+06/+08=state far pointer`; fill from `DS:2070` | `RET 0x06` | `lezac_state45_init(state, fill_word, &loop_index)` |
| `15afd..15b85` | `+04=unused static link`; map pointers/tables/globals described below | `RET 0x02` | `lezac_collect_tile(tile, cell, above_cell, score_values, bonus_values, state)` |

### `0x10000`: alternating checksum

At `0x1002c` the source byte is loaded and zero-extended. The loop index starts
at one; `AND AX,1` selects word `ADD` at `0x10045` or word `SUB` at `0x10050`.
Thus the existing accumulator is updated by `+b0-b1+b2-b3...`, modulo 65,536.
The routine does not zero the accumulator. Count zero leaves it unchanged;
count is unsigned and can reach 65,535.

The source offset word in its argument slot is incremented at `0x10053`;
the segment does not advance when that word wraps. The C API instead receives
a flat, non-overlapping byte span and returns the accumulator. The differential
cases use source offset zero and counts through 65,535, so no source wrap or
source/output overlap occurs. These ABI-only behaviors are not reconstructed.
The name describes the arithmetic; no cryptographic property is claimed.

### `0x106ab`: animation initializer, **not sound**

The stores at `0x106c8..0x10701` produce exactly:

```text
byte offset:   0       1      2     3        4      5     6
meaning:      frame  start   end  counter  delay   mode  step
initial:      start  start   end  delay    delay   mode   1
```

The exact original store order is `0,1,2,4,5,6,3`; the C implementation keeps it.
The counter intentionally starts at `delay`, not zero. Upper bytes in the
Pascal parameter slots are ignored (the tests deliberately set them nonzero).

The name is supported by the actor-update routine: `0x1606e` selects actor
record `+0x16`, `0x16088..0x1609a` advance/check counter and delay,
`0x160aa..0x160be` add a signed frame step, and `0x1613e..0x16156` use the
frame to index the graphics-related table. Mode 1 loops to start, mode 2
reverses the step at the endpoints (`0x1610c..0x16137`), and mode 3 restores
seven bytes from actor `+0x1d` (`0x160ea..0x16105`). Those update routines are
naming evidence only; they are **not** part of this C subset.

### `0x15715`: wrapping byte addition, then unsigned clamp

`ADD byte ptr ES:[DI],AL` at `0x15725` wraps before the subsequent byte load and
unsigned `JBE` at `0x15731`. The result is
`min((value + delta) mod 256, limit)`, not saturating addition. For example,
`(250,10,100)` returns **4**, not 100. All inputs are interpreted as byte bit
patterns; a negative-looking delta byte is still modulo-256 arithmetic.

### `0x15b86`: friction step with the minimum-integer quirk

This helper modifies the caller's velocity local at `SS:[frame-0x0c]`. The
actor caller pushes BP at `0x16b51`; actor `+6` is loaded into this local at
`0x16289..0x16290`, and other actor paths update the same local as movement.
The helper reduces speed toward zero, hence the descriptive name “friction.”

`CWD; XOR AX,DX; SUB AX,DX` at `0x15b97..0x15b9a` computes 16-bit absolute
value; signed `JGE` compares it to 43. Inputs from -42 through 42 become zero.
Other negative inputs gain 42; positive inputs lose 42. **-32,768 also becomes
zero**, because its 16-bit absolute value stays `0x8000`, negative to the signed
comparison. The C implementation handles this case explicitly without relying
on signed overflow. All 65,536 signed-word inputs are differential-tested.

### `0x12e8d`: 45-byte state initializer

The deliberately neutral name avoids inventing the full game record type.
The loop writes ten little-endian fill words at offsets `4,6,...,22` and ten
at `24,26,...,42`; fill comes from `DS:2070`. It then sets byte 44 to 2 and
zeros words at offsets 0 and 2. Every one of the 45 bytes is initialized.
`DS:2082`, used as the loop counter, finishes at **9**, not 10. The explicit
`loop_index` output preserves that global side effect. The unused static-link
argument is removed from the C API. Caller `0x12efd` initializes records at
`DS:785a` and `DS:7888`; do not infer a 45-byte array stride from this write size.

### `0x15afd`: tile collection

The original reads the tile at `[DS:c1fe]:[DS:c1e8]`. With `i=DS:c1e8`, it
reads the current word at `[DS:206e]:((2*i) mod 65536)` and the word one stride
before it at `[DS:206e]:((2*i-DS:2066) mod 65536)`. These two word reads occur
before any word-map clearing. The C caller supplies the selected current cell
and the already-read preceding/above cell, rather than exposing DOS segments.
Tests exercise high indices, shifted-index wrap, and subtracting-stride wrap
in the original machine's adapter.

Before checking the tile, the routine clears `DS:79a3` (collected tile),
`DS:2072` (bonus value), and `DS:661e` (above-present flag). It preserves the
incoming `DS:2074` accumulator if the tile is not collected. Signed byte
comparisons at `0x15b24` and `0x15b28` accept exactly IDs `0x67..0x72`.

For those twelve IDs, with `k=tile-0x67`:

1. Record the original tile ID in `DS:79a3`.
2. Read the bonus value from the twelve-word table at `DS:001a+2*k`.
3. Add the twelve-word table value at `DS:0002+2*k` to `DS:2074`, modulo 65,536.
4. Set the above-present flag iff the preceding word lies in `1..0x7fff`.
5. If the current word is below `0x8000`, clear both it and the tile. Otherwise
   leave that word unchanged and set the tile to `0xff`.

`lezac_collection` exposes these four global outputs. Its `score` field is the
16-bit accumulated award, **not the whole player's score**: caller instructions
at `0x16e19..0x16e24` later add it to a two-word record. `bonus` is a table-driven
value used by the caller's object-spawning path, not assumed to be bonus points.
The tile/collection name is justified by range selection, table lookup, map
clearing, and the actor caller. Original Pascal identifiers remain unknown.

## Reproduce and validate

From the repository root, with a C99 compiler and Python environment containing
`unicorn==2.1.4` and `pytest==9.1.1` (also listed in
`recovered/requirements-test.txt`):

```sh
PYTHONDONTWRITEBYTECODE=1 .venv/bin/python -m pytest -q -s -p no:cacheprovider tests/test_recovered.py
PYTHONDONTWRITEBYTECODE=1 LEZAC_CFLAGS='-fsanitize=undefined -fno-sanitize-recover=all' .venv/bin/python -m pytest -q -s -p no:cacheprovider tests/test_recovered.py
```

The fixture compiles real C into a temporary shared library under `TMPDIR`
(fallback: `/opt/data/cache/scratch` in this workspace), loads it using ctypes,
and compares its outputs with independently executed original instructions.
The compiler invocation is:

```text
cc -std=c99 -Wall -Wextra -Werror -pedantic -O2 -fPIC -shared [LEZAC_CFLAGS] recovered/lezac.c -o <scratch-directory>/liblezac.so
```

`CC` can select another compiler executable. The C has no Unicorn dependency;
only its differential-test harness needs Unicorn. The harness uses POSIX/GCC
shared-library options and is not itself a tested Windows build system.

### TDD record and case counts

Tests and the emulator harness were written first. The first exact normal test
command above returned **6 failed, 1 passed**: each C differential test failed
with `Portable C reconstruction is not implemented yet`, while the genuine
original-code loader/stack-check smoke test passed. C was implemented only after
that RED run. Normal and undefined-behavior-sanitized runs then returned
**7 passed**, with no compiler warnings or sanitizer diagnostics.

| Routine | C-versus-original cases per complete run | Coverage |
|---|---:|---|
| Alternating checksum | 293 | 35 fixed, 256 seeded random, 2 maximum-count/overflow sequences |
| Animation initializer | 1,137 | 625 boundary tuples, 512 seeded random; byte-exact state and guards |
| Wrapping add/clamp | 96,512 | All 65,536 value/delta pairs at limit 127; 121 boundary pairs at every limit |
| Friction | 65,536 | Every signed 16-bit value |
| 45-byte initializer | 519 | 7 boundary fills, 512 seeded random; full state, guards, global loop counter |
| Tile collection | 9,728 | All 256 tile IDs × 6 current-word classes × 5 above-word classes; 2,048 seeded random |
| **Total** | **173,725** | Semantic differential cases, not pytest test-item count |

There is one additional original-only loader smoke invocation, so the harness
prints **173,726 original stack-check executions** and **96,513** original calls
to `0x15715`. Native-only explicit regression assertions are not included in the
differential counts. Guard bytes check adjacent writes for checksum, animation,
byte arithmetic, friction, and the 45-byte record. Tile tests compare all four
observable global outputs and both selected map words.

## Deliberate limitations

- This is routine-level equivalence for explicit valid input domains, not a proof
  of full-program equivalence, original source recovery, or game compatibility.
- No main loop, rendering, audio, DOS services, resources, keyboard handling,
  complete record layouts, or reconstructed executable are supplied.
- Inputs and output buffers must be valid and non-overlapping. Original behavior
  when buffers alias each other, globals, stack arguments, code, or lookup tables
  is outside the C contract. No concurrent/interrupt-driven mutation is modeled.
- The flat checksum API does not model segment-offset wrap or mutation of its
  original source-pointer stack slot. State/animation arrays likewise must fit
  in valid caller-provided memory; malformed far pointers are not reproduced.
- The tile API reconstructs a selected-cell operation, not the segmented map
  storage engine. Its word/table pointers use native C words; the 45-byte record
  uses explicit little-endian serialization. No unaligned C word access is used.
- Register clobbers, flags, stack-local writes, runtime error exits, insufficient
  stack, and transient store ordering under interrupts are not C API outputs.
  The original normal stack-check path is genuinely exercised, not ported.
- Only friction is exhaustive over its complete scalar domain. Other routines
  combine boundaries and deterministic random cases; not every possible state,
  checksum payload, table, pointer, or three-byte clamp tuple is tested.
- Validation ran on Linux with GCC 14.2.0, Python 3.14.7, Unicorn 2.1.4, and
  pytest 9.1.1. The C99 code is intended to be portable, but no big-endian host,
  16-bit C compiler, different CPU architecture, or actual DOS hardware was run.
