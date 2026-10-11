# Pascal reconstruction of the six verified routines

`pascal/LZCORE.PAS` reconstructs the understood operations as Turbo Pascal-style
source: `unit`/`interface`/`implementation`, typed and untyped `var` parameters,
packed records, nested procedures, 16-bit `Integer`/`Word`, and Pascal function-name
result assignment. This is **not the author's source**, a complete game, or a
claim that a proprietary Turbo/Borland Pascal compiler emits identical bytes.
The tested compiler is **Free Pascal 3.2.2 in TP mode**.

`pascal/LZBRIDGE.PAS` is explicitly **modern Linux host verification scaffolding**.
It exports the six existing C-ABI symbol names for ctypes, but delegates every
operation to Pascal. It contains no C implementation and does not use the C
compiler or link `recovered/lezac.c`. Its native pointers are not DOS far pointers.

## Evidence and reconstruction boundary

The authoritative evidence is the original executable's bytes and their
execution. Retained `normalized_result.assembly` provides navigation into these
REA/Ghidra dossiers. Do not assume its rendered operands are infallible: the
[segment-register audit](../analysis/segment-register-audit.json) independently
flags 23 presentation mismatches among 65 checked register-direct MOVs; raw
exports remain unchanged. These six routines are checked against actual code:

- [`10000.json`](../analysis/rea/verified/functions/10000.json): checksum.
- [`106ab.json`](../analysis/rea/verified/functions/106ab.json): animation initializer.
- [`15715.json`](../analysis/rea/verified/functions/15715.json): byte addition/clamp.
- [`15b86.json`](../analysis/rea/verified/functions/15b86.json): nested friction helper.
- [`12e8d.json`](../analysis/rea/verified/functions/12e8d.json): 45-byte initializer.
- [`15afd.json`](../analysis/rea/verified/functions/15afd.json): collection helper.
- [`16053.json`](../analysis/rea/verified/functions/16053.json): actor caller context,
  including the parent velocity local and signed animation step.

The earlier C files and [behavioral documentation](recovered-routines.md) are
cross-checks, not the Pascal source specification. Descriptive Pascal identifiers,
unit boundaries, adapter declarations, and formatting are newly chosen.

The executable oracle is `original/game/LEZAC.EXE`, pinned to SHA-256:

```text
7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec
```

[`tests/dos_machine.py`](../tests/dos_machine.py) loads the original MZ, applies
all 468 relocations, protects the loaded code from guest writes, and executes
its real normal-path stack check. It checks returned CS:IP, SP/BP restoration,
stack-check counts, the entire relocated image, and the file hash. No routine
or runtime guard is replaced with a handwritten oracle or a stub.

## Source contracts and original parameter order

All six original entries use near returns. Offsets in this table are relative
to BP after `PUSH BP; MOV BP,SP`; far addresses are offset then segment.
Byte value parameters occupy word stack slots, with only their low byte used.
The first three public Pascal procedures retain the inferred declaration order.

| Original entry | Pascal declaration / reconstruction | Original stack and cleanup |
|---|---|---|
| `0x10000` | `AlternatingChecksum(var Source; var Accumulator: Word; Count: Word)` | `+4` count; `+6/+8` accumulator; `+10/+12` source; `RET 10` |
| `0x106ab` | `AnimationInit(var State: TAnimState; StartFrame, EndFrame, Delay, Mode: Byte)` | `+4` mode; `+6` delay; `+8` end; `+10` start; `+12/+14` state; `RET 12` |
| `0x15715` | `WrappingAddClamp(var Value: Byte; Delta, Limit: Byte)` | `+4` limit; `+6` delta; `+8/+10` value; `RET 8` |
| `0x15b86` | Nested parameterless `ApplyFriction` in a minimal `FrictionStep` facade | `+4` parent-frame link; velocity at `SS:[parent-12]`; `RET 2` |
| `0x12e8d` | Nested `Initialize(var Target: TInitState45)` in `State45Init` | `+4` unused static link; `+6/+8` state; fill at `DS:2070`; counter at `DS:2082`; `RET 6` |
| `0x15afd` | Nested parameterless `Collect` in `CollectTile` | `+4` unused static link; selected maps, tables, and outputs are globals; `RET 2` |

The last three public facades provide missing parent/global context; **their
public signatures are not claimed to be the original signatures**. In particular:

- `FrictionStep(Velocity: Integer): Integer` places velocity in an enclosing local,
  calls a nested parameterless procedure, and assigns `FrictionStep := LocalVelocity`.
  Only the nested operation is recovered; this facade is not the original actor
  update routine or an assertion about the parent's exact stack layout.
- `State45Init(var State; FillWord; var LoopIndex)` supplies the two original
  globals explicitly. Its inner initializer keeps the record argument and nested
  Pascal structure. The original static link was unused; the reconstructed
  context uses its outer arguments instead of fixed DS addresses.
- `CollectTile(var Tile; var Cell; AboveCell; var ScoreValues, BonusValues;
  var State)` supplies selected cells, two twelve-word tables, and scattered global
  values. `Collect` has no explicit arguments. This is a selected-cell
  reconstruction, not the original segmented map-storage implementation.

No public entry promises binary compatibility with the original call sites.
FPC's compact i8086 memory model is used for the DOS compile check (near code,
far data); this is not evidence of an identical compiler ABI or code generator.

## Data layouts

Host-compiled Pascal tests measure these types, rather than merely assuming that
ctypes declarations match them:

| Type | Size / layout | Evidence boundary |
|---|---|---|
| `Integer`, `Word`, `Byte` | 2, 2, 1 bytes in the tested TP-mode build | Verified by a compiled Pascal program |
| `TAnimState` | 7 bytes: frame, start, end, counter, delay, mode, signed step at offsets 0..6 | Seven stores plus actor-update context |
| `TInitState45` | 45 bytes: two words at 0/2; ten-word arrays at 4/24; mode byte at 44 | Precisely the initialized region, not a proven enclosing record size or stride |
| `TWordTable12` | 24 bytes, twelve words | Accepted tile range and two-byte table addressing |
| `TCollectionState` | 6 packed bytes: score/bonus words, collected/above bytes | **New adapter aggregate** of separate globals, not a recovered original record |

The two state addresses `DS:785a` and `DS:7888` do not establish a 46-byte record
size. The initializer proves writes through offset 44 only. Likewise, neither
the source's `packed` spelling nor its array lower bounds can be recovered
uniquely from machine code; the declarations enforce the observed byte layout.

## Arithmetic and state details retained

### Checksum (`0x10000`)

The untyped source and `var` accumulator are retained, in source-before-output
parameter order rather than the bridge's seed-first C ABI. A `Word` loop runs
from one through count; `Odd(I)` selects addition versus subtraction of the
zero-extended byte. Existing accumulator contents are not discarded. Word
assignment wraps modulo 65,536, including maximum-count cases.

`TByteView = array[0..0] of Byte` is an intentionally unchecked view of the
untyped buffer, **not an inferred original array bound**. With range checking
disabled, the valid supplied storage must contain `Count` bytes. Count zero
reads nothing; the bridge still calls the core's empty-loop path when its host
source pointer is nil, using an unused local byte.

### Animation (`0x106ab`)

Stores retain the order `0,1,2,4,5,6,3` and yield
`[start,start,end,delay,delay,mode,1]`. The counter starts at delay, not zero.
The seven-byte record is animation state; the actor caller supports that name.

### Byte addition/clamp (`0x15715`)

A byte assignment precedes the unsigned comparison. Thus `250 + 10` becomes
`4` before a limit of `100` is considered. This is not saturating addition.
`{$R-}` and `{$Q-}` are deliberate parts of the arithmetic contract.

### Friction (`0x15b86`)

The nested procedure uses `Integer(Abs(LocalVelocity)) < 43`. The explicit
16-bit narrowing is essential on a host that may widen intermediate expressions:
the original `CWD; XOR; SUB` leaves the absolute value of `-32768` as `$8000`,
which is negative in the subsequent signed comparison. Consequently `-32768`
and `-42..42` become zero; other values move toward zero by 42. All signed-word
inputs are tested against the original instructions.

### Initializer (`0x12e8d`)

Each iteration writes one fill word into each ten-word array; it sets byte 44 to
2 and then zeroes words 0 and 2. The externally visible index ends at **9**.
An explicit stopping condition avoids relying on an unspecified post-`for`
control-variable value. The `FillWord` context is unchanged.

### Collection (`0x15afd`)

The three reset outputs are cleared before signed-byte tile comparisons. Only
`$67..$72` enter the collection path; `ShortInt(Tile)` preserves signedness.
The selected bonus word is assigned and the selected award word is added to
the score accumulator modulo 65,536. Above-present is 1 only for words
`1..$7fff`. A current word below `$8000` is cleared along with its tile; a
high-bit current word stays intact and the tile becomes `$ff`. Both word values
are captured before any clearing. Noncollectible tiles preserve the score and
map values, while the three reset outputs still become zero.

## Reproduction

From the repository root, with the existing Python test dependencies and local
FPC installations from this workspace:

```sh
export FPC=/opt/data/tools/fpc-3.2.2/lib/fpc/3.2.2/ppcx64
export FPC_UNITS=/opt/data/tools/fpc-3.2.2/lib/fpc/3.2.2/units/x86_64-linux/rtl
export FPC8086=/opt/data/tools/fpc-3.2.2-downloads/cross/lib/fpc/3.2.2/ppcross8086
export FPC8086_UNITS=/opt/data/tools/fpc-3.2.2-downloads/cross/lib/fpc/3.2.2/units/msdos/8086-compact/rtl
PYTHONDONTWRITEBYTECODE=1 .venv/bin/python -m pytest -q -s -p no:cacheprovider tests/test_pascal_core.py
```

These paths are configuration, not compiled-in assumptions. Set `FPC` and
`FPC_UNITS` to another host installation; `FPC8086` and `FPC8086_UNITS` select
the cross compiler and matching compact RTL. Unit paths can be separated with
the host path separator. If no cross compiler is configured or found, only the
cross-compile test is explicitly skipped. A missing host compiler is a failure,
not a silently skipped differential test.

The fixture logs every invocation. The native library command uses:

```text
$FPC -n -Mtp -B -O2 -Fu$FPC_UNITS -Cg -Fupascal -FU<build> -FE<build> -o<build>/liblezac_pascal.so pascal/LZBRIDGE.PAS
```

The cross-compile test uses:

```text
$FPC8086 -n -Mtp -B -O2 -Fu$FPC8086_UNITS -Tmsdos -WmCompact -XX -FU<build> -FE<build> pascal/LZCORE.PAS
```

Actual paths are absolute in the fixture. The shared build driver resolves the
compiler and RTL; `-n` avoids global FPC configuration when an explicit/discovered
RTL is provided. A packaged compiler without an explicit RTL uses its normal
configuration. The canonical variables are `FPC`/`FPC_RTL` and
`FPC_DOS`/`FPC_DOS_RTL`; the earlier variables shown above remain aliases.
`-XX` accommodates the supplied smartlinked RTL. No generated PPU/object/library
or executable is placed in `pascal/`; temporary build directories are below
`build/pascal-tests/` and are removed after the run. The cross test compiles the
unit and checks that PPU/object files exist; it does **not** execute a rebuilt
DOS program. `tests/test_pascal_probe.py` now supplies that separate execution
layer for all seven reconstructed operations; see [pascal-recovery.md](pascal-recovery.md).

## TDD and measured coverage

The new tests were written before either Pascal source file. The initial run
returned **8 failed, 1 passed**: six differential groups, the layout test, and the
DOS compile test failed with `Pascal reconstruction is not implemented yet`;
the original-only loader smoke test passed. After implementation, the suite
returned **9 passed**, including the configured i8086 compile test.

[`tests/test_pascal_core.py`](../tests/test_pascal_core.py) directly calls the
unchanged case functions in [`tests/test_recovered.py`](../tests/test_recovered.py)
with its own FPC-built shared-library fixture. It never invokes that module's
C-build fixture. Each group additionally asserts its actual original-instruction
invocation count and verifies the original image remains unmodified.

| Routine | Pascal-versus-original cases |
|---|---:|
| Checksum | 293 |
| Animation | 1,137 |
| Wrapping addition/clamp | 96,512 |
| Friction | 65,536 |
| State initializer | 519 |
| Collection | 9,728 |
| **Total** | **173,725** |

The additional original-only smoke call yields **173,726 original stack-check
executions** and **96,513 original calls to `0x15715`**. Native-only regression
assertions and the compiled layout program are not counted as differential
cases. Layout output is `2 2 1 7 45 24 6`; selected offsets are `6 4 24 44 5`.

## Gaps and excluded behavior

- Only these six operations are reconstructed; no full parent actor routine,
  event loop, rendering, audio, input, DOS services, or playable game is supplied.
- Proprietary Turbo/Borland Pascal compilation, original identifiers/source,
  original instruction selection, exact register/flags effects, and binary
  linkage into the original EXE have not been established.
- Flat host buffer access does not reproduce the checksum's increment of its
  source-offset stack slot or segmented offset wrap. Counts through 65,535 are
  tested at offset zero in the oracle so the input remains within one segment.
- Valid, non-overlapping storage and stable context are required. Aliasing,
  invalid pointers, interrupted/concurrent mutations, and transient ordering
  visible to interrupts are outside the contract.
- Tile index/stride wrapping is exercised in the original oracle adapter. The
  Pascal selected-cell API receives already resolved cells and does not itself
  implement that segmented address computation.
- Original normal stack-check execution is verified, but runtime-error exits,
  inadequate stacks, and original parent/global memory addresses are not ported.
- Packed word storage was tested on little-endian x86-64 and compiled for i8086.
  No big-endian host or proprietary 16-bit compiler has been tested here.
- Friction is exhaustive over its scalar domain; the other groups reuse bounded
  exhaustive domains, boundary cases, and deterministic random cases. Passing
  them is not a proof for every possible buffer, table, or entire-game state.
