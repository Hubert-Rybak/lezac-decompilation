# Reproducing the analysis

## 1. Dependencies

The checked-in evidence came from:

- REA npm package **6.3.0**, pinned by `package-lock.json`.
- Ghidra **12.1.4**, official release archive
  `ghidra_12.1.4_PUBLIC_20260921.zip`.
- Eclipse Temurin **21.0.12.1+1**, 64-bit full JDK.
- Node.js **26.7.0** on Linux x64. REA also admits Node 22.19+ or 24.11+ in
  those release lines; its own package engine declaration is authoritative.

REA is the CLI/MCP orchestration layer. Ghidra performs the native disassembly
and decompilation. Neither sends the binary to an external analysis service in
this workflow. Obtain tools from their official upstream repositories:

- <https://github.com/morluto/rea>
- <https://github.com/NationalSecurityAgency/ghidra/releases/tag/Ghidra_12.1.4_build>
- <https://github.com/adoptium/temurin21-binaries/releases/tag/jdk-21.0.12.1%2B1>

Verify archive hashes against `analysis/provenance.json` before extracting.
An extraction program must retain executable permission on Ghidra's launchers
and host-native `decompile` binary; Python's basic ZIP extraction does not.

## 2. Import the original release

```sh
python3 tools/fetch_original.py
# Or import your matching local archive without a network request:
python3 tools/fetch_original.py --archive /path/to/lezac.zip
```

The importer validates the ZIP size, SHA-256, ZIP CRC, member list, and every
extracted file's size and SHA-256. It refuses path traversal and refuses to
replace different existing files. It places the originals under ignored
`original/game/`; it does not put them in Git.

## 3. Readiness and complete export

```sh
npm ci
export GHIDRA_INSTALL_DIR=/absolute/path/to/ghidra_12.1.4_PUBLIC
export JAVA_HOME=/absolute/path/to/jdk-21.0.12.1+1
export GHIDRA_HEADLESS_MAXMEM=768M
export REA_GHIDRA_STARTUP_TIMEOUT_MS=180000
npx --no-install rea doctor --provider ghidra --json

# Choose a NEW output directory: the exporter never overwrites prior evidence.
node tools/analyze.mjs original/game/LEZAC.EXE build/rea-new
```

`build/` must exist before choosing an output directory beneath it (`mkdir -p
build`). The exporter pins the input hash, opens one persistent REA stdio MCP
session, verifies the loaded image, enumerates the whole discovered inventory,
exports every function, checks input immutability, and closes the provider.
Each function dossier is saved separately, so a later failure does not erase
successful results. Failure counts result in a nonzero exit status.

Large dossiers are recovered through REA's retained-evidence export, keyed by
exact Evidence ID, input hash and function address. They are not truncated.
The full actor dossier at `0x16053` exercises this fallback in the checked-in run.

On the analysis host, the explicit Ghidra doctor scope was healthy. Doctor's
broader host check reported Debian 13 outside its advertised distribution list;
the actual Ghidra analysis nevertheless ran successfully. This is an observed
host result, not a claim that REA supports every Linux distribution.

## 4. Verification and a rebuilt C library

```sh
python3 -m venv .venv
. .venv/bin/activate
python -m pip install -r requirements-dev.txt
npm test
python -m pytest tests -q
LEZAC_CFLAGS='-fsanitize=undefined -fno-sanitize-recover=all' \
  python -m pytest tests/test_recovered.py -q
make all
```

`make all` builds `build/liblezac.a` and `build/liblezac.so` from the **six recovered
routines only**. It does not create a DOS game executable. Test fixtures compile
and execute that same C source through ctypes against Unicorn execution of the
original MZ-relocated instructions, not a second handwritten implementation.

To regenerate the evidence index:

```sh
python tools/summarize_analysis.py
```

CI authenticates a fresh original download, validates the committed evidence,
builds the C library, runs all tests, and repeats the C comparisons with UBSan.
It does **not** download Ghidra and redo the full decompilation on every push.
The separate command in section 3 performs that operation.

## Address conventions

REA reports linear addresses at load segment `0x1000`. For this MZ image:

```text
analysis_linear = 0x10000 + relative_segment*16 + offset
file_offset     = analysis_linear - 0x10000 + 0x770
```

The file-offset formula applies only inside the initialized MZ module. Loader
relocations alter segment words in memory, so original file bytes and relocated
memory bytes can differ. REA's `inspect_native_load_image` verifies that mapping.
Function body range ends are **inclusive**; segment ends are exclusive. Shared
function tails must be counted by union, not added twice.

Decompiler output is preserved with warnings and imperfect inferred types.
Unchanged input hashes and green tests prove the scoped checks described in the
documents, not original-source recovery or whole-game equivalence.
