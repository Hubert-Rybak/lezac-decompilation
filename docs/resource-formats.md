# Verified resource formats

`tools/asset_formats.py` implements bounded, read-only byte parsers for the
locally supplied Larax & Zaco resources. This is format recovery, not a playable
port or recovery of the original Pascal source. `tests/test_assets.py` contains
synthetic contracts and optional read-only specimen regressions; it embeds no
original assets or score names.

**Important:** the seven level containers are complete, but all fourteen declared
compressed planes underfill their requested output sizes. The API preserves the
known output and reports the deficit. The strict full-plane decoder rejects
these inputs; it never pads, guesses missing tiles, or emulates unsafe reads.

## Evidence and address convention

Source paths below are repository-relative. The verified REA dossiers retain
assembly as well as inferred pseudocode; format decisions use the assembly and
independently parsed local bytes, not inferred types alone.

| Evidence | Repository-relative source | Relevant observations |
|---|---|---|
| E1 | `analysis/rea/verified/functions/1909c.json` | Sprite count, byte dimensions, width-times-height payload reads |
| E2 | `analysis/rea/verified/functions/10709.json` | 768-byte palette read and BIOS DAC block installation |
| E3 | `analysis/rea/verified/functions/12597.json`, `analysis/rea/verified/functions/116ca.json` | Score reader/writer: count, four-byte score, nine-byte name storage field |
| E4 | `analysis/rea/verified/functions/10630.json` | Sound count word and six-byte stride |
| E5 | `analysis/rea/verified/functions/10c33.json` | Level field order, output sizes, trailing record strides |
| E6 | `analysis/rea/verified/functions/182d0.json` | Nibble-RLE runs, unbounded input reads, inclusive output stores |
| E7 | `analysis/rea/verified/functions/18bb4.json`, `analysis/rea/verified/functions/112db.json` | Ordinary sprite transparency and separate font recoloring behavior |
| E8 | `original/game/LEZAC.EXE`, load-image `0000:1033–1082` | Sound stop comparison and the two tick thresholds; independently disassembled original bytes |

Executable `segment:offset` references use the **unrelocated MZ load image**.
Its header is `0x770` bytes. EXE file offsets are
`0x770 + 16 * segment + offset`; REA addresses are
`0x10000 + 16 * segment + offset`. For example, the decompressor at
`082d:0000` is EXE offset `0x8a40` and REA address `0x182d0`.
E8 is a raw instruction-range reference, not a claim that REA exported an
interrupt-handler entry dossier. Input fingerprints are pinned in
`tests/test_assets.py`; the executable fingerprint is
`7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec`.

## SPR: variable-size indexed images

```text
u8 count
repeat count times:
    u8 width
    u8 height
    byte pixels[width * height]
EOF
```

The records have no padding or compression. Pixels are row-major, left-to-right,
top-to-bottom. Counts and exact EOF are verified for `BOMOMIMK.SPR` (91 images,
20,168 bytes), `PROVA.SPR` (91 images, 21,250 bytes), and `FONTS.SPR` (68 images,
5,425 bytes): 250 images total. [E1]

Record sizes vary: `PROVA.SPR` zero-based images 47–52 are 1×1 placeholders.
`FONTS.SPR` contains 26 10×10 images followed by 42 8×8 images; its raw indices
are 0, 1, and 255. Offsets cannot be reused between the two 91-image files.

The ordinary blitter skips index **0**, not 255. The font renderer treats index 1
as a foreground-color placeholder and treats index 0 according to background
mode; other indices are copied. The contact sheet uses ordinary transparency
and the initial palette without font recoloring. It is an inspection view, not
an exact rendering of every in-game text/palette state. [E7]

## PAL: initial RGB6 palette

`BOMPAL.PAL` is exactly 768 bytes: 256 consecutive RGB triples, each component
in 0–63, without a header. `parse_pal` retains these values. [E2]
`rgb6_to_rgb8` applies `(value * 255 + 31) // 63` for previews; this rounded
0–255 mapping is an export convention, not a claim about analog DAC behavior.

## DAT: fixed-width score records

```text
u8 count
repeat count times:
    u32le score_bits
    u8 name_length
    byte name_storage[8]
EOF
```

`RECS.DAT` has seven records and is exactly 92 bytes. The length must not exceed
8 but does not change the record stride of 13. All eight storage bytes are
preserved, including bytes beyond the logical name. No text encoding or trimming
is imposed. `score_raw_u32` preserves the bits; the original declaration's
signedness is not established from positive specimen scores. The inspection
CLI prints only the score-record count, never names or name storage. [E3]

## SON: tone/tick records, not PCM

```text
u16le count
repeat count times:
    u16le tone_parameter
    u8 gate_ticks
    u8 step_ticks
    byte opaque_tail[2]
EOF
```

`PROEFS.SON` contains 130 records in 782 bytes, including 15 records with
`tone_parameter == 30000`. This value stops playback; it is not a 30 kHz note.
`gate_ticks` and `step_ticks` are descriptive names for the two consumed
thresholds, not recovered identifiers. The two remaining bytes are preserved
without labeling them as pointers or padding. [E4, E8]

The absolute tick duration and a complete playback model remain unverified.
No audio renderer, guessed millisecond conversion, or WAV export is provided.
The parser does not impose observed timing relationships as universal validity
rules.

## SCH: concatenated level records

There is **no leading count or offset table** in `LIVELS.SCH`. Its first word is
width, not a level count. All words below are little-endian. [E5]

```text
u16 width; u16 height
u8 counted_tile_id; u16 required_count; u8 opaque_header_byte
u16 compressed_size_A; byte compressed_A[compressed_size_A]
u16 compressed_size_B; byte compressed_B[compressed_size_B]
u16 opaque_word_0; u16 opaque_word_1
u8 count_30; byte records_30[count_30][30]
u8 count_7;  byte records_7[count_7][7]
u8 count_14; byte records_14[count_14][14]
```

The container parses seven records to EOF at 41,047. Record offsets are
`0, 1242, 4923, 10984, 15225, 21546, 36128`; these agree with the executable's
separate seven-word-pair index at EXE offset `0xcc1a`.
The descriptive header names are interpretations, and the remaining header,
words, and trailing records remain opaque. [E5]

Plane A requests `width * height` bytes; B requests twice that number. This does
not establish B as another visual tile map. Each compressed triple is
`control, a, b`, expanding to `a` repeated `(control >> 4) + 1` times, then `b`
repeated `(control & 15) + 1` times. Incomplete triples are rejected. [E5, E6]

| Level | Dimensions | A known / required | A missing | B known / required | B missing |
|---|---|---|---:|---|---:|
| 1 | 60×33 | 1971 / 1980 | 9 | 3948 / 3960 | 12 |
| 2 | 100×53 | 5275 / 5300 | 25 | 10595 / 10600 | 5 |
| 3 | 150×60 | 8989 / 9000 | 11 | 17989 / 18000 | 11 |
| 4 | 100×58 | 5791 / 5800 | 9 | 11585 / 11600 | 15 |
| 5 | 110×62 | 6807 / 6820 | 13 | 13610 / 13640 | 30 |
| 6 | 180×64 | 11511 / 11520 | 9 | 23018 / 23040 | 22 |
| 7 | 140×52 | 7271 / 7280 | 9 | 14548 / 14560 | 12 |

The original decompressor does not enforce its compressed-input length. Its
inclusive store loops also write one extra byte per run; the next run normally
overwrites that byte. The final store can exceed the requested output boundary.
The safe decoder implements only logical run output. Exact missing tails would
require establishing original runtime memory state, not zero-filling the file's
short payloads. No level preview or repair mode is offered. [E6]

## Python API and strictness

The standard-library-only module exposes:

- `parse_spr(data)`: list of `{index, offset, width, height, pixels}`.
- `parse_pal(data)`: list of 256 RGB6 tuples; `rgb6_to_rgb8(entry)` converts one.
- `parse_scores(data)`: `{offset, score_raw_u32, name_length, name_storage,
  name_bytes}` records; byte storage is retained without character decoding.
- `parse_son(data)`: `{index, offset, tone_parameter, gate_ticks, step_ticks,
  opaque_tail, stop}` records.
- `rle_prefix(payload)`: complete logical expansion of only the supplied triples.
- `rle_strict(payload, required)`: exactly the requested bytes, or `ValueError`
  for underfill. As in the original, reaching the requested boundary may truncate
  a run or leave complete triples unused. **Strict means bounded and no missing
  output**, not exact consumption of every logical run. Framing is validated
  even when `required == 0`.
- `parse_levels(data)`: complete container records with byte offsets/end,
  header fields, `opaque_words`, `words_offset`, two `planes`, and `trailers`.
  Each plane retains `{offset, compressed, prefix, required, missing, excess}`;
  `missing` and `excess` are nonnegative. Each trailer retains its `stride`,
  `count_offset`, and raw `records`.

Malformed/truncated counted containers and unconsumed trailing bytes raise
`ValueError`. SCH consumes complete records until EOF, so another valid record
is not garbage. A partial final record is rejected. Empty count-prefixed files
must still contain their count field; empty SCH denotes zero records. Zero
sprite/level dimensions are rejected as an explicit supported-input policy,
not a recovered original-program check. Parsers accept byte strings and do not
try to authenticate unknown historical variants; specimen tests pin hashes.

## Local inspection and preview

From the repository root:

```sh
.venv/bin/python -m pytest tests/test_assets.py -q
.venv/bin/python tools/inspect_assets.py
.venv/bin/python tools/inspect_assets.py --contact-sheet build/assets/sprite-contact-sheet.png
```

`--asset-dir PATH` selects another local directory; the default is
`original/game`. The default CLI only reads files and prints JSON counts plus
per-level deficits. Exit 0 means successful **inspection**, not complete level
recovery. File/format errors produce an error and exit 2. There is no network,
server, download, upload, or publishing behavior.

The optional contact sheet requires Pillow. Its PNG is created only under the
repository's non-symlinked `build/`; existing files, direct symlinks, and resolved
paths escaping that directory are refused. Relative output paths are relative
to the repository, not the caller's working directory. It shows all sprite
records, labeled by source stem and zero-based index, using the initial palette.
The generated sheet is a local derivative of original assets: keep it ignored,
do not commit or publish it. Choose a new filename to make another preview.

Synthetic tests run without originals; the two specimen tests skip explicitly
when local resources are absent. With originals present, they require matching
fingerprints, exact container round trips (including opaque bytes), all fourteen
strict underfill rejections, and unchanged source bytes afterward. Synthetic
coverage includes every RLE control byte, output-boundary behavior, malformed
counts/lengths/dimensions, every nonempty truncation of a complete synthetic
level, CLI privacy/error behavior, and preview path/overwrite constraints.
