# Decompilation scope and findings

## Evidence, not recovered original source

The analyzed target is the `LEZAC.EXE` in the pinned **Larax & Zaco v1.0
shareware** archive. The input is a 52,384-byte MZ executable with a 1,904-byte
header, 468 relocations, and a 50,480-byte initialized load module. The original
entry is `0000:7783`; at REA's load segment `1000h`, its linear address is
`0x17783`. See [provenance](../analysis/provenance.json).

REA **6.3.0**, installed from npm with its lockfile, used Ghidra **12.1.4** and
`x86:LE:16:Real Mode`. The inspected upstream main revision is recorded separately;
it was not the version executed. The Borland copyright string, Pascal-style
shortstrings, far runtime calls, and callee-cleanup calling conventions support
a **Borland Pascal-family compiler inference**, not an exact compiler version.

The complete provider-discovered inventory contains **215 procedures**:

- `CODE_0`: 87 procedures in the main game module.
- `CODE_1`: 2; `CODE_2`: 5; `CODE_3`: 18; `CODE_4`: 10; `CODE_5`: 93.
- The ancillary segments contain runtime and support/graphics code; do not
  classify all 128 ancillary procedures as compiler runtime.

Every procedure has an original REA Evidence record, raw C-like pseudocode,
assembly, function body ranges, call/reference observations, and source identity.
All **94 loaded-image checks** passed, including the module mapping and loader
relocations. This attests the import, not gameplay correctness. The original EXE
remained unchanged and the REA session closed successfully.

## Important export pitfall resolved

The actor-update dossier at `0x16053` exceeds REA's MCP response budget because
its evidence includes detailed references and p-code. The exporter recovers the
**exact retained Evidence ID** through `export_evidence_bundle`; it does not
truncate the function, retry a different provider, or call a shortened result
complete. `transport-16053.json` records the diagnostic and recovered ID.
The complete dossier represents 5,934 instruction-body bytes.

REA's assembly field is an array of instruction lines, not a string. The
exporter tests cover this boundary and retained-evidence identity validation.

## Navigation: analyst-inferred roles

These are descriptive labels, not symbols recovered from original source.
The address is the stable identity; raw pseudocode keeps Ghidra's names.

- `0x10000`: alternating-byte checksum, also used over embedded text.
- `0x10630`: load the counted six-byte sound-record stream.
- `0x106ab`: initialize seven-byte animation state.
- `0x10709`: load and install the 256-entry VGA DAC palette.
- `0x10c33`: load a level and its two compressed planes and record arrays.
- `0x116ca`: write the seven-entry high-score file.
- `0x12361`: menu/input dispatch.
- `0x12597`: load or initialize high-score data.
- `0x126e8`: game initialization, memory allocation and interrupt setup.
- `0x12adc`: initialize a level's actors, graphics and counters.
- `0x12efd`: initialize a new game.
- `0x1370e`: mark a structure/tile as moving and allocate motion records.
- `0x1432a`: update platform/motion descriptors.
- `0x145fa`: update individual moving-tile/debris records.
- `0x15102`: update grouped moving structures.
- `0x15715`: wrapping byte addition followed by an unsigned upper clamp.
- `0x15afd`: collect a bonus-coded tile and update map/counter state.
- `0x15b86`: horizontal velocity friction step.
- `0x16053`: actor update, including players, animation and interactions.
- `0x17783`: entry/main control flow.
- `0x182d0`: nibble-run decompressor.
- `0x1909c`: indexed sprite resource loader.
- `0x196df`: Pascal runtime stack-space guard.

Follow the matching `analysis/rea/verified/functions/<address-without-0x>.json`
for instruction-level justification. The table is a navigation aid; only routines
listed in [recovered-routines.md](recovered-routines.md) have a compiled and
differential-tested portable implementation.

## Known limits and remaining work

1. **Not a playable or bit-identical rebuild.** Generated pseudocode is not
   compilable C and original variable names, source structure, types and comments
   were not recovered. Only the separately named `recovered/` subset is compiled.
2. **215 is discovery coverage, not proof of all executable code.** Dynamic
   interrupt/vector targets and callbacks can be missed. Initialization registers
   timer/keyboard handlers at relative `0000:0fbe` and `0000:1091`; these are not
   separate entries in the default function inventory. Resource research inspected
   the timer instructions separately. Inline data and unclassified gaps remain.
3. Ghidra emits explicit warnings in five pseudocode outputs: `0x17783`,
   `0x184ce`, `0x18917`, `0x192e2`, `0x192e9`. Keep those warnings; successful export
   is not successful semantic reconstruction of every function.
4. DOS services, IRQ timing, VGA presentation, PC-speaker timing, self-modification,
   heap allocation order, and whole-game behavior are not established by static
   decompilation or isolated-routine tests.
5. **Level files expose original out-of-bounds behavior.** The seven level records
   frame correctly, but all fourteen compressed planes underfill the requested
   output. The original decompressor continues reading past their payloads and
   performs an extra overlapping store. Strict decoders preserve the known prefix
   and reject full decoding instead of inventing a tail. The 29 tests in
   `tests/test_rle_machine.py` execute the entire original MZ-relocated routine
   and its unmodified stack guard: 14 planes under two controlled buffer-tail
   patterns, plus one complete synthetic payload. Memory hooks observe only the
   source and destination buffers; global hooks interfered with far returns in
   the tested Unicorn setup. No instruction patch or stack-check stub remains.
   Exact runtime tails still need the original heap/buffer state; see
   [resource-formats.md](resource-formats.md).
6. Several resource fields remain opaque; absolute sound timing and complete
   `CARO.CAR`, `GRAN.MST`, and `SFONLEF.ZBG` interpretation remain open.

A useful next milestone is an emulated full-game state capture followed by typed
actor/map reconstruction and interrupt-handler recovery. This is outstanding
work, not a claim of completion hidden behind a test count.
