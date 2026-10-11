# Original-style Pascal recovery and execution

## What was actually recovered

The original has a Borland runtime marker, Pascal shortstrings, callee-cleaned
16-bit calls, hidden static links, and compatible runtime/file layouts. This
supports a Turbo/Borland Pascal-family origin, **not an exact compiler version**.
No original `.PAS`/unit/debug names or author source files were identified by the
bounded inspection described in [pascal-layouts.md](pascal-layouts.md).

The reconstruction now contains:

- `pascal/LZCORE.PAS`: six operations in Pascal, including packed records,
  original-style `var` parameter order and genuinely nested helpers. Three
  outer facades supply unrecovered parent/global context; they are explicitly
  not the original complete enclosing procedures.
- `pascal/LZRLE.PAS`: the original-style unsafe decompressor at `0x182d0`.
- `pascal/LZTYPES.PAS`: sixteen measured storage layouts, plus documented array
  views. Names, grouping and Pascal declaration spelling are analyst choices.
  Unknown bytes remain real fields rather than discarded padding.
- `LZBRIDGE.PAS` and `LZRLEBR.PAS`: **modern native test bridges**, not original
  game units or DOS-compatible external ABIs.
- `LZPROBE.PAS`: a **new computational verification program**, not a recovered
  main program, game launcher, or playable game.

The prior C reconstruction remains available as a separate representation.
Pascal verification executes original machine instructions; C is not its oracle.

## Compiling and running

Use Free Pascal **3.2.2**. A packaged native `fpc` may use its normal configuration.
For a standalone installation, set `FPC` to the compiler executable and `FPC_RTL`
to the directory containing `system.ppu`. For the DOS cross compiler, set:

```sh
export FPC_DOS=/absolute/path/to/lib/fpc/3.2.2/ppcross8086
export FPC_DOS_RTL=/absolute/path/to/lib/fpc/3.2.2/units/msdos/8086-compact/rtl
make pascal-native PYTHON=.venv/bin/python
make pascal-dos PYTHON=.venv/bin/python
.venv/bin/python -m pytest tests -q
```

`tools/build_pascal.py` resolves explicit variables, then PATH, then the known
optional reference-workspace installation if present. It verifies compiler
version and output magic, uses separate generated-unit directories, and never
changes global configuration. `FPC_UNITS`, `FPC8086`, and `FPC8086_UNITS` remain
supported aliases for the initial core-test configuration. The RLE host test uses
`FPC`/`FPC_RTL`; prefer those canonical names for all components.

The [toolchain manifest](../analysis/pascal-toolchain.json) pins the official
retrieved native/cross archives, sizes and SHA-256s. The CI workflow shows the
hash-checked extraction of only the cross compiler and compact-model units.
Compilers, archives, generated units, libraries and EXEs are not committed.

The DOS flags are `-Mtp -Tmsdos -WmCompact -XX`. The measured compact build has
`Integer=2`, `Word=2`, `LongInt=4`, `Pointer=4`; the native TP-mode probe has
`2,2,4,8`. Small-model DOS data pointers were only two bytes, so that model was
not used for the final far-data tests. Matching pointer width does **not** make
FPC output binary-compatible with the original compiler. `-XX` accommodates the
archive's smartlinked `system.a` runtime.

`make pascal-native` builds two host shared libraries and an executed probe.
`make pascal-dos` builds `build/pascal-dos/LZPROBE.EXE` and executes its actual
MZ-relocated instructions in Unicorn. Failure, unknown DOS services, or exhausted
instruction budget is a nonzero result. The Python tests also compare that
program's results with original instructions, not just a hardcoded PASS banner.

## Verification layers — do not conflate them

1. **Native Pascal versus original DOS instructions:** the unchanged six-routine
   oracle corpus performs **173,725 comparisons**. It covers every signed-word
   friction input, byte arithmetic boundaries, buffer/state output and collection
   quirks. The actual original runtime stack guard executes; code remains protected
   and unchanged. See [pascal-core.md](pascal-core.md).
2. **Native Pascal RLE versus original DOS instructions:** **1,052 comparisons**:
   256 controls times four requested lengths, plus seven levels times two planes
   times two controlled memory tails. This is a bounded, non-segment-wrapping
   arena contract, not every possible pointer/count combination.
3. **Newly compiled i8086 DOS execution:** `tests/test_pascal_probe.py` runs the
   rebuilt MZ with a limited DOS-service fixture. Its seven routines are compared
   against **12 original invocations** (including five friction values and two
   RLE requests). This is a small cross-target execution check, not a claim that
   all 174,777 native differential comparisons ran on the i8086 build.
4. **Compiled type layouts:** sixteen sizes plus selected field offsets are
   measured in both native and actually executed DOS builds. Odd-position words,
   actor byte `$05`, Pascal `string[8]`, and the known 45-byte score prefix are
   retained. The 46-byte separation between two globals is not falsely asserted
   as the record's size. `TFile128` is an original-runtime view, not FPC's file type.
5. Existing C/evidence/asset tests remain regression coverage, not substitutes for
   the Pascal checks. Optional DOS tests skip only if no cross compiler exists;
   an explicitly configured but broken compiler/RTL fails instead.

## The DOS execution fixture is deliberately limited

`tools/run_dos_probe.py` is **not a full DOS emulator**. Unicorn executes the
compiled guest's instructions. The fixture supplies a synthetic PSP/environment,
conventional-memory area, and only the observed INT 21h services: version 5.0,
get/set interrupt vector, standard-handle device query, unsupported-LFN response,
stdout/stderr writes, and exit. The DOS version is fixture input, not an observed
host OS. There is no guest filesystem, BIOS/VGA/audio hardware, real timer,
keyboard, or gameplay validation. It does not execute original game startup.

## Unsafe decompressor compatibility

The recovered Pascal deliberately retains these observed behaviors:

- `PackedLength` is unused; input is consumed until enough output is produced.
- Each triplet supplies two nibble run lengths plus two values; both lengths add
  one, but each store loop also includes its upper endpoint.
- Consequently a run stores an extra byte; the next run can overlap that store.
- The first run executes even when `Required=0`.
- All fourteen real level planes underfill and can read beyond their payload.
  The tests put the payload in controlled arenas and compare the actual memory
  consequences with original execution. They do not invent missing level bytes.

`LegacyExpandUnsafe` must not be used as an untrusted-input asset decoder. The
separate Python decoder remains bounded and rejects underfilled output. The host
bridge and tests require sufficiently allocated, non-overlapping arenas without
16-bit offset wrap; invalid pointers and exhausted storage are outside scope.

## REA assembly presentation corrections

A narrow independent decoder of opcode `8C/8E` plus register-direct ModRM found
**23 mismatches among 65 checked instructions**. For example, original bytes
`8e da` at `0x19203` and `0x19b21` load DS from DX, whereas retained REA text
reverses the operands. `8e c0` at `0x107d2` similarly loads ES from AX.

The audit is reproduced by:

```sh
.venv/bin/python tools/audit_segment_moves.py
```

Its complete result is [segment-register-audit.json](../analysis/segment-register-audit.json).
Raw REA dossiers, assembly and pseudocode are preserved unchanged. This is only
an audit of unprefixed 16-bit register-direct segment MOVs, **not a validation of
all other rendered operands or all decompiler semantics**. Original bytes and
actual instruction execution take precedence.

## Still unrecovered

Original identifiers, exact source syntax, unit names, compiler release/options,
full parent procedures and globals, main loop, complete actor physics/rendering,
audio/input/interrupt behavior, DOS integration, and exact level-tail memory state
remain unresolved. This work recovers a tested original-language subset and
storage structure; it is neither the author's source archive nor a complete or
bit-identical DOS game rebuild.
