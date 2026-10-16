# Lezac decompilation

Evidence-backed reverse engineering of **Larax & Zaco v1.0 shareware**
(`LEZAC.EXE`), using [REA](https://github.com/morluto/rea) **6.3.0** and
Ghidra **12.1.4**.

**Status: 215 default-discovered procedures exported; seven routines reconstructed
in original-style Pascal, sixteen packed storage layouts, and the prior six C
reconstructions. Native differential tests and a rebuilt 16-bit DOS computational
probe pass. The ORIGINAL game has a pinned DOSBox-X reference baseline, with level 1
completed under it. This is NOT a playable rebuild, a bit-identical rebuild, or the
author's original source files.**

## What is here

- [Original-style Pascal recovery](docs/pascal-recovery.md): six core routines
  plus the legacy RLE decompressor, with **174,777 native Pascal-versus-original
  comparisons**. A separate compiled i8086 MZ probe executes all seven operations
  in a limited DOS-service fixture and compares twelve original invocations.
- [Sixteen packed-record layouts](pascal/LZTYPES.PAS), compiled and measured on
  native and DOS targets: actors, animation, debris, scores and resource records.
  Names are inferred; unknown bytes are preserved. Exact author syntax and
  compiler version are not recovered.
- **215 function dossiers, 215 C-like pseudocode files and 215 assembly files**
  under [`analysis/rea/verified/`](analysis/rea/verified/). They retain REA Evidence
  IDs, input identity, inclusive body ranges, calls, references and limitations.
- **94 successful loaded-image checks**, including the original file mapping and
  DOS relocation processing. Input SHA-256:
  `7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec`.
- [Six portable C routines](recovered/lezac.c): checksum, animation initialization,
  wrapping add/clamp, friction, state initialization and tile collection.
  **173,725 C-versus-original x86 cases**, including every signed 16-bit friction
  input; the same cases pass with GCC's undefined-behavior sanitizer.
- **29 additional original-code decompressor probes** characterize source
  overread and overlapping extra stores without patching game instructions.
- Resource-format research and strict decoders for sprites, palette, scores,
  sound records and level containers. All 250 sprites and seven level records
  are accounted for; uncertain level tails are reported rather than invented.
- Pinned input provenance, a verified original-file importer, repeatable REA
  exporter, coverage index, tests, and CI.
- [Byte-decoded assembly audit](analysis/segment-register-audit.json): 23 reversed
  operand presentations among 65 checked register-direct segment MOVs. The raw
  REA export is unchanged; original executable bytes take precedence.
- [Original-game emulator baseline](docs/emulator-baseline.md): a hash-pinned
  DOSBox-X 2024.03.01 with a fixed configuration. `tools/run_original.py` runs the
  authenticated original from disposable copies, records input traces and exact VGA
  framebuffer checkpoints, and re-verifies the originals after every run. The
  committed run summaries cover a level-1 completion, death and restart, and
  two-player controls. Gameplay replays are host-timed, not cycle-exact.

Original EXE/ZIP/resource files and generated image previews are **not committed**.
See [RIGHTS.md](RIGHTS.md); archival availability is not a public-domain claim.

## Start here

- [Original-game emulator baseline: setup, observed behavior, level inventory](docs/emulator-baseline.md)
- [Pascal recovery, build targets, verification and limits](docs/pascal-recovery.md)
- [Pascal core contracts and original calling conventions](docs/pascal-core.md)
- [Record layouts, static links and compiler/source archaeology](docs/pascal-layouts.md)
- [Analysis, function roles and limitations](docs/analysis.md)
- [Reproduction instructions and tool versions](docs/reproduction.md)
- [Recovered C: original ABI, exact behavior and test domains](docs/recovered-routines.md)
- [Resource formats and the level-decoder defect](docs/resource-formats.md)
- [Machine-readable input/tool provenance](analysis/provenance.json)
- [All 215 functions as CSV](analysis/functions.csv)
- [Coverage measurements](analysis/coverage.json)

Raw decompiler `.c` files are intentionally **not compiled**. They contain
Ghidra pseudotypes, imperfect signatures and preserved warnings; the separate
`recovered/` directory is the actual compilable C subset.

## Quick verification

Requires Python 3.12+, a C99 compiler, Free Pascal 3.2.2, make, Node.js compatible
with REA, and npm. DOS tests additionally need the i8086 cross compiler; see the
Pascal build instructions above. Missing optional cross tooling is reported as
a skip, not as a passing DOS validation.

```sh
python3 -m venv .venv
. .venv/bin/activate
python -m pip install -r requirements-dev.txt
npm ci
python tools/fetch_original.py
make all
make pascal-native PYTHON=python
make test PYTHON=python
LEZAC_CFLAGS='-fsanitize=undefined -fno-sanitize-recover=all' \
  python -m pytest tests/test_recovered.py -q
```

`make all` produces `build/liblezac.a` and `build/liblezac.so`, not a game
executable. The importer can use `--archive /path/to/lezac.zip` for an offline
copy; it validates every original file and refuses to overwrite different files.
With the cross compiler configured, `make pascal-dos PYTHON=python` builds and
executes `LZPROBE.EXE`. This is a new verification harness, not `LEZAC.EXE`.

To rerun REA after installing Ghidra and a full JDK:

```sh
export GHIDRA_INSTALL_DIR=/absolute/path/to/ghidra_12.1.4_PUBLIC
export JAVA_HOME=/absolute/path/to/jdk-21
export GHIDRA_HEADLESS_MAXMEM=768M
export REA_GHIDRA_STARTUP_TIMEOUT_MS=180000
mkdir -p build
node tools/analyze.mjs original/game/LEZAC.EXE build/rea-new
```

Use a new output directory. The exporter processes the whole discovered
inventory and fails on missing results. Oversized MCP responses are recovered
from their exact retained Evidence records, not truncated.

## What remains

- Recover dynamic targets such as the registered timer and keyboard interrupt
  handlers; **215 discovered routines is not proof of exhaustive code discovery**.
- Resolve remaining actor/map field meanings and reconstruct complete parent
  routines, physics, rendering, audio, input and DOS services; then test gameplay.
- Establish actual heap/buffer state for the missing tails of all fourteen
  compressed level planes. A safe decoder must not silently reproduce the
  original out-of-bounds accesses or fabricate replacement bytes.
- Make original-game replays cycle-exact and capture machine state. The pinned
  DOSBox-X delivers input in host time, and its save states abort while the game
  holds a file open. Both need a separately pinned, patched emulator (see the
  emulator baseline).

CI checks evidence integrity and the reconstructed subset. It does not make the
unreconstructed remainder correct, and it does not rerun Ghidra on every push.
