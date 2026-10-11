# Larax & Zaco 1.0 — Pascal declaration/layout archaeology

## Scope and confidence

This is a read-only investigation of `original/game/LEZAC.EXE`, the retained REA assembly, and selected original resource bytes in `/opt/data/lezac-decompilation`. It proposes **layout-compatible Turbo Pascal declarations, not recovered original source text**. All declaration and field names below are analyst-created. No production source, tests, or commits were changed. The research phase did not compile declarations. Integration now places these views in `pascal/LZTYPES.PAS`; `tests/test_pascal_layouts.py` checks compiled sizes/selected offsets natively and in an executed i8086 DOS probe. They have not been substituted into the original game.

Evidence classes:

- **Measured:** instruction operand width, effective address, stride, byte-copy/read length, control-flow comparison, or directly parsed specimen framing.
- **Inferred:** purpose/name, grouping into a source record, original array bounds, original Pascal type spelling, or compiler/unit attribution.
- **Unknown/raw:** bytes whose meanings or signedness have not been established. Preserve them; do not synthesize zero padding.

Where `Word` is used for an opaque 16-bit field, it is a storage choice, not proof that the original declaration was unsigned. Conversely, explicit sign extension and signed comparison are reported when present. A pointer-shaped argument does not by itself distinguish `var R: TRecord` from a by-value typed pointer.

## 1. Input identity and address convention

Measured from the local EXE:

| Property | Value |
|---|---|
| Length | 52,384 bytes |
| SHA-256 | `7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec` |
| MZ header | `0x770` bytes / 1,904 bytes |
| Relocation count | 468 |
| Initialized load module | 50,480 bytes |
| Entry, before relocation | `0000:7783` |
| REA load base | `0x10000` |
| Data segment before relocation | `0x0aa2` |
| Data segment at this load base | `0x1aa2`, linear base `0x1aa20` |

In this report, code addresses such as `0x16053` are **REA linear addresses**, not file offsets. Within the initialized image:

```text
file_offset = linear_address - 0x10000 + 0x770
DS-address linear = 0x1aa20 + DS_offset
```

A DS address above initialized DS offset `0x1b0f` is not present as initialized data in the EXE. In particular, the actor and debris arrays are runtime storage, not tables that can simply be dumped from the EXE file. `GRAN.MST` does contain serialized actor records.

Primary instruction evidence is in `analysis/rea/verified/functions/<entry>.json`, under `normalized_result.assembly`. The report uses instruction addresses, not the decompiler's guessed C types or synthetic local-variable offsets. All 215 dossier filenames were counted; main-module assembly was searched for record-stride and relevant global-address references. Detailed inspection was focused, not a claim of complete semantic recovery of all 215 functions.

### Independent raw-byte correction

Capstone disassembly of the original relocated bytes found a material presentation error in the retained REA assembly:

- At `0x19203`, bytes `8e da` mean **`MOV DS,DX`**, while the REA text says `MOV DX,DS`. Together with the relocated `MOV DX,0x1aa2` at `0x19200`, this establishes DS.
- At `0x19b21`, bytes `8e da` again mean **`MOV DS,DX`**, restoring DS in the Move-like runtime helper. The preceding `0x19b10` is `8c da`, genuinely `MOV DX,DS`.

Use original bytes to resolve segment-register direction; do not treat every REA mnemonic as authoritative merely because operand widths elsewhere look plausible.

## 2. Actor record: exactly 38 bytes

### Measured extent, storage and indexing

Three independent mechanisms establish stride `0x26`, rather than just proximity of two globals:

1. `0x13001..0x13010` computes `DS:1bae + extra_index * 0x26`. `0x12fad..0x12fb7` checks the extra-actor count against `0x1e` and increments it before allocation.
2. `0x133a2..0x133bb` copies the next extra actor over a removed actor with a **`0x26`-byte** call to `0x19b0e`.
3. The resource loader `0x10902..0x1091f` reads `count * 0x26` bytes directly into consecutive extra actors.

The two player records are `DS:1b88` and `DS:1bae` (`0x12d91..0x12e08`); extra actor 1 is `DS:1bd4`. A useful overlay is 32 records at `DS:1b88`, ending just before `DS:2048`. This does **not** prove that the source declared one array of 32: two separately declared players followed by `array[1..30]` is also consistent. The resource loader's unchecked bulk count is not evidence that malformed inputs safely respect the spawn routine's 30-extra-actor limit.

Player accesses commonly use `DS:1b62 + player_number * 0x26`, with player numbers 1 and 2; extra-actor accesses use `DS:1bae + extra_index * 0x26`, with extra indices starting at 1. These biased bases are not actual starts of additional records.

### Actor field map

Offsets are hexadecimal relative to the actor's first byte.

| Offset | Width / interpretation | Instruction evidence and confidence |
|---|---|---|
| `00` | Byte; kind/state discriminator | Loaded into local kind at `0x162b6..0x162bc`; ranges and kinds branch at `0x162c5..0x162d7`, `0x1758c..0x175a4`. Not a Boolean. Meaning/name inferred. |
| `01` | Byte; render-sprite slot | `0x16064` loads it; `0x16245..0x16253` scales it by 8 into `DS:c21e`. Distinct from animation frame. |
| `02` | Byte; reused counter/link | Written from spawn parameter at `0x1304c..0x13052`; decremented/tested at `0x175a7..0x175c6`; resource loading also adjusts it as a sprite-relative link for some actors at `0x10bb2..0x10bc1`. Do not name it universally “lifetime.” |
| `03` | Byte; animation selector A | Written from a lookup at `0x17bc6..0x17bda`; consumed at `0x1728f` and by loader `0x10bd5..0x10bf4`. Direction/animation-set purpose inferred. |
| `04` | Byte; animation selector B | Written `0x17bde..0x17bf2`; consumed `0x172e3`. |
| `05` | **One unknown byte** | Preserved by the 38-byte copies and resource read. No scalar interpretation established. It is not safe to discard as alignment padding: the seven `GRAN.MST` records contain values `0, 2, 6, 6, 191, 0, 2` here. |
| `06` | Signed 16-bit horizontal velocity | Read `0x1628c`, written `0x1757e`; local is used by signed friction `0x15b93..0x15bc3` and signed movement `0x17400..0x1741b`. `Integer` is a good source-level reconstruction. |
| `08` | Signed 16-bit vertical velocity | Read `0x16296`, written `0x17588`; signed comparisons and gravity at `0x17022..0x17045`; integration at `0x173e5..0x173fd`. |
| `0a` | **Word**, low byte used as horizontal subpixel fraction | Low byte read `0x16278`; at `0x17560..0x17568`, low-byte local is zero-extended and **stored as a word**. Do not collapse this to one byte plus unproven padding. |
| `0c` | **Word**, low byte used as vertical subpixel fraction | Low byte read `0x16282`; zero-extended word write `0x1756c..0x17574`. |
| `0e` | Word; kind-dependent parameter bits | Read/masked as two byte-sized links at `0x1636a..0x16399`; also a countdown at `0x1697c`, `0x16a4a..0x16a76`, and a divisor at `0x170bc..0x170dc`. No single semantic name/type beyond width. |
| `10` | Word; kind-dependent parameter bits | `0x163a4..0x163b6`, `0x170c6`; initialized for a player state at `0x1312b`. |
| `12` | Word; kind-dependent parameter bits | `0x170d0`; two-level nested access `0x15bda..0x15be9`; loader adjusts a group/root reference at `0x10a1e..0x10a2f`. |
| `14` | Signed byte; vertical origin adjustment | Explicit `CBW` at `0x162a0..0x162a4` and `0x1751a..0x1751e`. Derived as `16 - sprite_height` at `0x15ade..0x15af5`. |
| `15` | Byte; behavior/player role | Read at `0x1615d`; 0/1 select the two players, other values select different behaviors. Not Boolean. |
| `16..1c` | 7-byte active animation | Pointer selected at `0x1606b..0x16075`; initializer and update below. |
| `1d..23` | 7-byte saved animation | Current-to-saved copy at `0x167ae..0x167c2`; saved-to-current copy at `0x160f1..0x16105`. |
| `24` | Byte; health/energy-like quantity | New game sets 100 at `0x12f30..0x12f35`; replenish/cap at `0x16e55..0x16e76`; damage and death at `0x174af..0x17513`. Also set to `0xfe` at `0x168b8`, so do not restrict its declared range to `0..100`. |
| `25` | Byte; origin/spawner/group ID | Assigned from spawner index `0x17c2c..0x17c32`; actor death replenishes a 30-byte source record through it at `0x174e6..0x174f9`; loader uses it for group membership at `0x10ac2..0x10acc`. |

A single fixed set of semantic names for `02`, `0e`, `10`, `12`, and `25` would overstate recovery. The binary is compatible with either a flat record with reused fields or a Pascal variant record; a variant declaration has not been established.

### Animation's seven bytes

`0x106c8..0x10701` writes exactly these fields in the order `0,1,2,4,5,6,3`:

| Offset | Proposed name/type | Evidence |
|---|---|---|
| `0` | `Frame: Byte` | Initialized from start; frame is zero-extended at `0x160b4..0x160b7`. |
| `1` | `FirstFrame: Byte` | Wrap target `0x160dd..0x160e4`. |
| `2` | `LastFrame: Byte` | Unsigned endpoint comparisons `0x160d4..0x160d8`, `0x16115..0x16119`. |
| `3` | `Counter: Byte` | Incremented `0x16088`; reset `0x160a2`. Initialized to delay, not zero. |
| `4` | `Delay: Byte` | Unsigned comparison at `0x16096..0x1609a`; advance only when incremented counter exceeds delay. |
| `5` | `Mode: Byte` | 0 disables updates; observed mode 2 reverses step; mode 3 restores saved state. Mode 1 follows ordinary wrap behavior. |
| `6` | `Step: Shortint` | Explicit sign extension at `0x160aa..0x160ae` and signed negation at `0x1612d..0x16137`. |

Frame number selects the four-byte sprite-definition table, then copies its bitmap offset into the actor's separate render record: `0x1613e..0x16156`. The record is animation, not a sound envelope.

## 3. Small debris/moving-tile records: 11 bytes

Measured base `DS:209e`: `0x1293d..0x12940` stores this address in `DS:207c`. Accesses use `DS:2093 + index * 11` with a one-based index (`0x14617..0x14622`, `0x14950..0x1495b`). Deletion compacts exact 11-byte elements (`0x14553..0x1456b`, `0x145b6..0x145ce`).

The individual-moving-tile allocation cap is 1,600 (`0x13753`, then increment at `0x1375e`), and the resulting extent ends at `DS:655e`. There are different banks/uses within this same record storage:

- Projectile allocation accepts count `< 0xc6` and increments it, giving indices up to 198 (`0x1403b..0x14046`).
- Level initialization sets the other last-used index to 199 (`0x12ba9`). Its update loop starts at 200 (`0x1492f..0x14954`).
- Do not assume index 199 is an active projectile, or that every field is initialized on both allocation paths.

| Offset | Storage | Evidence / interpretation |
|---|---|---|
| `00` | Word | Tile-map cell index. Stored from argument at `0x13792..0x13798`; used as map offset in `0x14628..0x1468b`. |
| `02` | Word | Tagged map/structure value; initialized with bit `0x8000` set at `0x137da..0x137e3`. The projectile initializer `0x14064..0x14106` does not establish a fresh value for this field. |
| `04` | Shortint-compatible byte | Horizontal velocity; copied to `DS:78d2` at `0x14631..0x14635`. Signed direction selection in raw helper `0x13ee4..0x13ee8`; actor interaction sign-extends it at `0x16fc5..0x16fc9`. |
| `05` | Shortint-compatible byte | Vertical velocity; copied to `DS:78d4` at `0x1463b..0x1463f`; signed direction at `0x13f0b..0x13f0f`; sign-extended at `0x16fd3..0x16fd7`. |
| `06`, `07` | Raw bytes | Horizontal/vertical motion phases copied to `DS:78d3/78d5` at `0x14645..0x14653`. Helper uses byte addition and **overflow flag**, `0x13eeb..0x13efc`, `0x13f11..0x13f22`; do not substitute ordinary carry-based unsigned fractions. |
| `08` | Byte | Age/lifetime/settling counter: initialized at `0x137d5`, or from parent local at `0x140ee..0x140f8`; update semantics differ between banks. |
| `09` | Byte | Tile code, copied from map at `0x137af..0x137be`; projectile initializer writes `0x75` at `0x140e9`. |
| `0a` | Byte | Auxiliary timer/flags; initialized at `0x137ea` or `0x140fc..0x14106`. Both countdown and high-bit uses exist in `0x145fa`; preserve full byte. |

The phase bytes can be modeled as `Byte` to preserve bits. The signed-overflow instruction behavior must be preserved separately; that choice does not prove their original Pascal declarations were `Byte`.

## 4. Grouped moving structures / large debris: 15 bytes

Measured base `DS:6620`, written to `DS:207a` at `0x12943..0x12946`. Allocation and update use `DS:6611 + index * 15` (`0x1397c..0x13988`, `0x1511e..0x15129`). Limit 250 is explicitly compared at `0x1396d`; record compaction uses stride 15 at `0x150a6..0x150be`. The 250-record extent ends exactly at `DS:74c6`, the next resource table's base.

| Offset | Storage | Evidence / interpretation |
|---|---|---|
| `00`, `02` | Words | Bounding start/end **byte offsets into the word map**, not two Cartesian coordinates. Initialized `0x1398b..0x1399a`; shifted right one at `0x1512f..0x15134`, `0x15162..0x15168`. |
| `04` | Word | Tagged structure/map identity, bit `0x8000` set at `0x1399e..0x139a7`; loaded into `DS:205e` at `0x1516e..0x15172`. |
| `06`, `07` | Signed bytes | Horizontal/vertical velocity. Initialization `0x139bb..0x139cb`; explicit `CBW` and absolute-value arithmetic at `0x15544..0x15558`. |
| `08`, `09` | Raw bytes | Motion phases; zeroed `0x139ab..0x139b6`; loaded `0x1514e..0x1515c`; same overflow-driven motion scheme as individual debris. |
| `0a` | Word | Previous sum of absolute component velocities; initial computation `0x139cf..0x139e8`; comparison with current sum `0x15544..0x15576`. Name inferred from arithmetic, not a physical-unit claim. |
| `0c` | Byte | Flags. Zeroed `0x139f7`; mask operations on indexed `DS:661d` occur in `0x15102`. |
| `0d` | Byte | Settling/age counter. Zeroed `0x139ef`; indexed `DS:661e` is updated in `0x15102`. |
| `0e` | Byte | Twice number of included map cells, truncated to byte at `0x13a38..0x13a42`; used as a weight in collision calculations. “Mass” is a useful inferred name, not a recovered type or invariant. |

## 5. Scores: signed comparison is recoverable

### High-score row, 13 bytes

The loader and writer use `DS:1af7 + index * 13` with the first real row at `DS:1b04`. Writer `0x116f4..0x11764` emits count 7, then 4 bytes from offset 0 and 9 bytes from offset 4. Reader `0x1266d..0x126aa` uses the same layout. String initialization calls the bounded shortstring-copy helper with maximum 8 (`0x125ed..0x125fe`). Thus `string[8]` is a strong source-compatible reconstruction, not an eight-byte C string.

**New signedness evidence:** `0x11877..0x11889` loads low/high score words, compares the high words with signed `JL/JG`, then compares the low words unsigned when the high words are equal. This is a signed 32-bit comparison, independently confirmed from original bytes. `0x11d10..0x11d24` has the same signed-high/unsigned-low pattern. Consequently **`Longint` is substantially better supported than an unsigned score interpretation**; earlier resource documentation intentionally left this unresolved from positive specimen values alone. Disk parsing should still preserve the four raw bytes for unusual values.

| Offset | Proposed type | Evidence |
|---|---|---|
| `00..03` | `Longint` | Four-byte I/O; signed 32-bit sorting as above. |
| `04..0c` | `string[8]` | One length byte plus eight storage bytes; bounded shortstring copy and fixed nine-byte I/O. Preserve storage bytes beyond logical string length. |

Seven rows occupy `DS:1b04..1b5e`. No score names were included in this report.

### Player score/display state: known 45-byte prefix, not proven 46-byte record

`0x12e8d` initializes a 45-byte object at both `DS:785a` and `DS:7888`. `0x12776..0x12780` explicitly computes their address difference, 46, into `DS:2084` for shared display code. This confirms a **46-byte address separation**, not `SizeOf(record)=46` or an array declaration.

| Offset | Storage | Evidence / proposed purpose |
|---|---|---|
| `00..03` | Longint-compatible score | Zeroed by `0x12ee2..0x12eea`; word award added with carry at `0x16e19..0x16e24`; passed through signed long-division runtime at `0x111c1..0x11219`. |
| `04..17` | Ten words | Fill loop `0x12eaf..0x12ebd`; current digit/glyph positions inferred from rendering/update `0x11259..0x11281`, `0x1129c..0x112c7`. |
| `18..2b` | Ten words | Fill loop `0x12ec1..0x12ecf`; target digit/glyph values built from repeated division by 10 at `0x111d3..0x11219`. |
| `2c` | Byte | Display state: initializer sets 2 at `0x12edd`, award sets 0 at `0x16e14`, digit conversion sets 1 at `0x11230`, completion sets 2 at `0x112ce..0x112d3`. |
| `2d` | **Outside the known initialized prefix** | Possible inter-variable alignment or an unrecognized trailing field; not included in proposed 45-byte type. |

The initializer's two loops each cover ten words, even though specific display paths visit fewer digit positions. Do not shrink those arrays to the visible digit count. The original loop counter global `DS:2082` ends at 9.

## 6. Graphics, level, sound and composite-resource records

### Sprite definition and active render record

- **Sprite definition: 4 bytes**, addressing `DS:c322 + sprite_number * 4`, with loader numbers starting at 1, so its first loaded entry is `DS:c326`. Width byte at `+0`, height byte at `+1`, bitmap **offset word**, not far pointer, at `+2`. Evidence `0x19162`, `0x19172`, `0x19194..0x191bf`; bitmap storage is in the separately held far buffer `DS:c1e4`.
- **Active render entry: 8 bytes**, addressing `DS:c21e + slot * 8`, slot 0 for the first player. X word at `+0`, Y word at `+2`, width byte at `+4`, height byte at `+5`, bitmap offset word at `+6`. Evidence `0x18fec..0x19046`; actor read/write `0x16245..0x16269`, `0x17530..0x17553`. Actor X/Y are therefore not fields at actor `+06/+08`; those fields are velocities.

Coordinates are preserved as `Word` bit patterns below: direct word transfer does not recover all original signedness. The bitmap segment is external to both four- and eight-byte records.

### Level trailing records

`0x10f02..0x10f95` separately reads three byte counts and these exact raw arrays. These are distinct from active actors, animation, debris and motion records:

| Stride | Destination / count | Recovered field use |
|---|---|---|
| 30 | `DS:74c6`, count `DS:79a6` | Spawn/configuration records, consumed at `0x17a89..0x17c32`. |
| 7 | `DS:771e`, count `DS:79a5` | Map ID word `+0`, X word `+2`, Y word `+4`, player-selection flags byte `+6`. `0x159cc..0x15a00` matches ID after clearing bit 15 of input; `0x1059a..0x105e5` selects coordinates by flags. Not the seven-byte animation type. |
| 14 | `DS:77ce`, count `DS:79a7` | Word lower/upper map-ID bounds at `+0/+2`, trigger ID word at `+4`, four source tile bytes at `+6..+9`, four replacement bytes at `+0a..+0d`. `0x15795..0x157a0` matches trigger ID; `0x157fb..0x15861` compares bounds and loops over four replacements. |

For the 30-byte record, widths and offsets are established by `0x17a98..0x17c32`:

```text
00 word X; 02 word Y; 04..07 raw/unknown
08 byte enabled-like flag
09 byte remaining counter; 0a byte available counter; 0b byte actor kind
0c/0e words base/span for actor+0e
10/12 words base/span for actor+10
14/16 words base/span for actor+12
18/19 bytes base/span for actor+24
1a byte behavior passed into actor+15
1b byte countdown; 1c byte countdown reload; 1d byte animation delay
```

“Span” means an argument supplied to the Random-like runtime, then added to a base; it is not a proven inclusive maximum. `0x17a9b` decrements `1b`; `0x17ad6..0x17add` reload it from `1c`; `0x17b38..0x17b3f` decrement counters `09/0a`; actor death increments the originating record's `0a` at `0x174f9`. Fields `04..07` remain opaque in this inspection.

### 16-byte motion descriptor and `GRAN.MST`

Do not confuse the 14-byte level replacement table with this **16-byte** record. Loader `0x10ae3..0x10b02` reads `motion_count * 16` at `DS:79ea + first_index * 16`, so descriptor 1 begins at `DS:79fa`. Updater `0x14353..0x14377` uses the same stride.

Measured fields in `0x1432a` and `0x15872`:

| Offset | Storage | Proposed role |
|---|---|---|
| `00`, `01` | Bytes | Two render-sprite references; loader adjusts both at `0x10b51..0x10b64`, deletion adjusts them at `0x1343b..0x1345b`. |
| `02` | Byte | Divisor/phase step, read `0x143ba`, `0x1442d`. |
| `03` | Byte | Mode/drag, `0xff` selects a separate branch (`0x1437d`, `0x158ab`). Not Boolean. |
| `04`, `05` | Bytes | Amplitude-like parameters, zero-extended at `0x14475..0x14479`, `0x144db..0x144df`. |
| `06` | Byte | Phase, updated and masked to 7 bits at `0x14438..0x14446`. |
| `07`, `09` | Signed words | Base/bias components, explicitly `CWD` at `0x1438a..0x1438e`, `0x143d1..0x143d5`. Their odd offsets establish packed field placement. |
| `0b`, `0d` | Words, role-dependent bits | Outputs stored `0x143ca`, `0x14423`, `0x144b2`, `0x14519`; used as velocity contributions in one mode (`0x158b8..0x158d3`) and absolute position in another (`0x1596a..0x1597f`). Keep neutral `Word` storage rather than asserting one meaning. |
| `0f` | Signed byte | Explicit `CBW` at `0x14416..0x1441a`; additive bias. |

Additional specimen result, not a new production parser: the literal at code `0x12ad3` is the counted filename `gran.mst`, passed to loader `0x108a5` at `0x12e7f..0x12e88`. Following that loader's actual read sequence frames the local file exactly:

```text
u8 actor_count = 7
7 * 38-byte actor records                 file [1,267)
7 * u8 sprite-definition indices          file [267,274)
7 * (word X, word Y)                      file [274,302)
u8 motion_count = 6                       file offset 302
6 * 16-byte motion descriptors            file [303,399)
EOF                                      file offset 399
```

The X/Y read loop is `0x10996..0x109f9`; the motion-count read precedes `0x10a19`. File SHA-256 is `3f0cf17706d557ca863d6e39bfa6641fba777a88815ba3f0a22e5ac89e0e39e2`. Actor byte `05` has four distinct observed values, including nonzero values, reinforcing the preservation requirement. Nonzero contents alone do not distinguish a meaningful field from reserved storage; neither interpretation permits discarding it without evidence. This format hypothesis does **not** frame `CARO.CAR` or `SFONLEF.ZBG` to their EOF; no reinterpretation of those files is claimed.

### Other serialized views

- **Sound: 6 bytes**. `0x10665..0x1068c` reads a word count, multiplies by six and reads into the far buffer at `DS:79c0`. Timer code `0x11014..0x11030` indexes `(note_index-1)*6`. Word `+0` is passed to the Sound-like runtime; value 30000 is a stop marker (`0x11036..0x11059`). Bytes `+2/+3` are timing thresholds (`0x11061..0x1106f`); `+4/+5` remain raw. These are not six-byte Pascal `Real` values merely because their size is six.
- **Palette: RGB triples**. Palette loader `0x10709` reads 768 bytes and installs the DAC block. Proposed RGB record is a three-byte disk view; original source could equally have used a byte array.
- **Level fixed header: 8 bytes**. `0x10cde..0x10d5b` performs separate reads of two word dimensions, a byte tile ID, a word required count, and another byte. A packed header view is useful for parsing, but those separate reads into globals do not establish that the original source had an aggregate header record. The compressed streams and variable trailing arrays cannot be represented as one fixed-size record.
- **File runtime storage: 128 bytes**. Local file objects occupy 128-byte regions (`0x116d9`, `0x116e8`); `0x1a7ca` assigns handle/mode and filename, `0x1a855` stores record size at `+4`, and `0x1a82f..0x1a837` accesses the zero-terminated filename at `+0x30`. The standard Borland `FileRec` declaration below is manual-derived and corroborated at those offsets, not an original game-defined type [M1, pp. 222–223]. Source code should normally declare `F: file`, not manipulate that internal record.

## 7. Proposed declarations

These are **reconstructed** Turbo Pascal storage declarations, integrated into `pascal/LZTYPES.PAS`. The explicit `packed` keyword expresses the intended layout; it is not evidence that the original author wrote it. Borland's manual documents automatic record packing, and global-variable word alignment separately [M1, pp. 28, 262–263]. In particular, word alignment between two globals is not a reason to insert padding between their record fields.

```pascal
type
  TAnimation7 = packed record
    Frame: Byte;                       { $00 }
    FirstFrame: Byte;                  { $01 }
    LastFrame: Byte;                   { $02 }
    Counter: Byte;                     { $03 }
    Delay: Byte;                       { $04 }
    Mode: Byte;                        { $05 }
    Step: Shortint;                    { $06 }
  end;

  TActor38 = packed record
    Kind: Byte;                        { $00 }
    SpriteSlot: Byte;                  { $01 }
    CounterOrLink: Byte;               { $02 }
    AnimationSelectorA: Byte;          { $03 }
    AnimationSelectorB: Byte;          { $04 }
    Unknown05: Byte;                   { $05: raw, no padding inference }
    VelocityX: Integer;                { $06 }
    VelocityY: Integer;                { $08 }
    FractionX: Word;                   { $0A: low byte used, word stored }
    FractionY: Word;                   { $0C: low byte used, word stored }
    Param0E: Word;                     { $0E: kind-dependent }
    Param10: Word;                     { $10: kind-dependent }
    Param12: Word;                     { $12: kind-dependent }
    OriginYAdjustment: Shortint;       { $14 }
    Behavior: Byte;                    { $15 }
    Animation: TAnimation7;            { $16..$1C }
    SavedAnimation: TAnimation7;       { $1D..$23 }
    Energy: Byte;                      { $24: includes sentinel values }
    OriginId: Byte;                    { $25: kind-dependent }
  end;

  TSpriteDef4 = packed record
    Width: Byte;                       { $00 }
    Height: Byte;                      { $01 }
    BitmapOffset: Word;                { $02: segment held elsewhere }
  end;

  TRenderSprite8 = packed record
    XBits: Word;                       { $00 }
    YBits: Word;                       { $02 }
    Width: Byte;                       { $04 }
    Height: Byte;                      { $05 }
    BitmapOffset: Word;                { $06 }
  end;

  TDebris11 = packed record
    CellIndex: Word;                   { $00 }
    MapTag: Word;                      { $02 }
    VelocityX: Shortint;               { $04 }
    VelocityY: Shortint;               { $05 }
    PhaseXBits: Byte;                  { $06 }
    PhaseYBits: Byte;                  { $07 }
    AgeOrLife: Byte;                   { $08 }
    TileCode: Byte;                    { $09 }
    AuxTimerOrFlags: Byte;             { $0A }
  end;

  TGroup15 = packed record
    FirstWordMapOffset: Word;          { $00 }
    LastWordMapOffset: Word;           { $02 }
    MapTag: Word;                      { $04 }
    VelocityX: Shortint;               { $06 }
    VelocityY: Shortint;               { $07 }
    PhaseXBits: Byte;                  { $08 }
    PhaseYBits: Byte;                  { $09 }
    PreviousSpeedSum: Word;            { $0A }
    Flags: Byte;                       { $0C }
    SettleCounter: Byte;               { $0D }
    Weight: Byte;                      { $0E }
  end;

  TScore13 = packed record
    Score: Longint;                    { $00..$03: signed comparison }
    Name: string[8];                   { $04..$0C }
  end;

  TScoreDisplay45 = packed record
    Score: Longint;                    { $00..$03 }
    CurrentGlyphs: array[0..9] of Word; { $04..$17 }
    TargetGlyphs: array[0..9] of Word;  { $18..$2B }
    DisplayState: Byte;                { $2C }
  end;

  TMotion16 = packed record
    PrimarySprite: Byte;              { $00 }
    ReferenceSprite: Byte;            { $01 }
    DivisorOrPhaseStep: Byte;          { $02 }
    ModeOrDrag: Byte;                  { $03 }
    AmplitudeX: Byte;                  { $04 }
    AmplitudeY: Byte;                  { $05 }
    Phase: Byte;                       { $06 }
    BaseX: Integer;                    { $07 }
    BaseY: Integer;                    { $09 }
    ResultXBits: Word;                 { $0B: velocity or position }
    ResultYBits: Word;                 { $0D: velocity or position }
    BiasY: Shortint;                   { $0F }
  end;

  TSpawn30 = packed record
    XBits: Word;                       { $00 }
    YBits: Word;                       { $02 }
    Unknown04_07: array[0..3] of Byte;  { $04..$07 }
    Enabled: Byte;                     { $08: not asserting Boolean }
    RemainingCount: Byte;             { $09 }
    AvailableCount: Byte;             { $0A }
    ActorKind: Byte;                   { $0B }
    Param0EBase: Word;                 { $0C }
    Param0ESpan: Word;                 { $0E }
    Param10Base: Word;                 { $10 }
    Param10Span: Word;                 { $12 }
    Param12Base: Word;                 { $14 }
    Param12Span: Word;                 { $16 }
    EnergyBase: Byte;                  { $18 }
    EnergySpan: Byte;                  { $19 }
    Behavior: Byte;                    { $1A }
    Countdown: Byte;                   { $1B }
    ReloadCount: Byte;                 { $1C }
    AnimationDelay: Byte;              { $1D }
  end;

  TMapPoint7 = packed record
    MapId: Word;                       { $00 }
    XBits: Word;                       { $02 }
    YBits: Word;                       { $04 }
    PlayerFlags: Byte;                 { $06 }
  end;

  TTileReplacement14 = packed record
    FirstMapId: Word;                  { $00 }
    LastMapId: Word;                   { $02 }
    TriggerMapId: Word;                { $04 }
    FromTiles: array[1..4] of Byte;     { $06..$09 }
    ToTiles: array[1..4] of Byte;       { $0A..$0D }
  end;

  TSound6 = packed record
    ToneParameter: Word;               { $00: 30000 stops }
    GateTicks: Byte;                   { $02 }
    StepTicks: Byte;                   { $03 }
    Unknown04_05: array[0..1] of Byte;  { $04..$05 }
  end;

  TRGB3 = packed record
    Red: Byte;                        { $00 }
    Green: Byte;                      { $01 }
    Blue: Byte;                       { $02 }
  end;

  TLevelHeader8 = packed record
    Width: Word;                      { $00 }
    Height: Word;                     { $02 }
    CountedTile: Byte;                 { $04 }
    RequiredCount: Word;               { $05: disk view, unaligned }
    Unknown07: Byte;                   { $07 }
  end;

  TFile128 = packed record
    Handle: Word;                     { $00 }
    Mode: Word;                       { $02 }
    RecSize: Word;                    { $04 }
    PrivateData: array[1..26] of Byte;  { $06..$1F; standard runtime }
    UserData: array[1..16] of Byte;     { $20..$2F; standard runtime }
    FileName: array[0..79] of Char;     { $30..$7F; zero terminated }
  end;
```

Measured storage-compatible array views, **not recovered source identifiers/bounds**:

```pascal
type
  TActorStorageView = array[0..31] of TActor38;
  TSmallMotionStorage = array[1..1600] of TDebris11;
  TGroupStorage = array[1..250] of TGroup15;
  THighScoreStorage = array[1..7] of TScore13;
  TLanguageTextView = array[0..1, 0..12] of string[255];
```

Do **not** declare the two score-display globals as `array[1..2] of TScoreDisplay45` and expect the second to land at `DS:7888`. Keep separately allocated globals or an explicitly documented address overlay; adding an unknown tail byte is an alternative storage view, not a proven field.

## 8. Likely Pascal source constructs and calling convention

### Strong nested-procedure evidence

These are explicit static links, not generic far data pointers:

| Child entry | Calling/access evidence | Source-level implication |
|---|---|---|
| `0x15b86` | Parent pushes BP at `0x16b51`; child reads `[BP+4]`, then `SS:[parent-0x0c]` at `0x15b90..0x15b93`; `RET 2`. | Parameterless nested friction helper in actor update, capturing the parent's local velocity. |
| `0x15a75` | `[BP+4]` is used to fetch parent actor argument `SS:[parent+4]` at `0x15a83..0x15a8a`; explicit byte parameter is at `[BP+6]`; `RET 4`. | Nested sprite/animation-selection helper with one byte-sized value parameter. |
| `0x15cb0` | `PUSH BP; CALL` at `0x16554..0x16555`; child's `0x15cbe..0x15cc8` gets parent actor. | Nested collision/group helper within actor update. |
| `0x15bcc` | `PUSH BP; CALL` in `0x15cb0` at `0x15fbd..0x15fbe`; `0x15bda..0x15be5` follows **two** `[frame+4]` links before reaching actor data. | Nested again inside `0x15cb0`, not a sibling that happens to have extra pointer parameters. |
| `0x1458d` | `0x145d8..0x145ee` reads/updates `SS:[parent-2]` via `[BP+4]`; explicit removal index is `[BP+6]`; `RET 4`. | Nested removal helper that adjusts its enclosing update loop's index. |
| `0x12e8d` | Caller pushes record far address, then BP at `0x12f8b..0x12f9a`; child record is `[BP+6/+8]`, `RET 6`, static link unused. | Nested initializer in new-game setup; unused link does not become a meaningful source argument. |
| `0x11845` | `0x11d2c..0x11d38` pushes score high/low, player number, then BP. | High-score insertion helper nested in `0x11b14`; `[BP+8/+10]` is its score argument. |

This is the near nested-call convention documented by Borland [M1, pp. 242–245]. A sensible **schematic** lexical hierarchy is:

```text
UpdateActor                            16053
  SelectSprite                         15a75
  CollectTile                          15afd (unused static link)
  ApplyFriction                        15b86
  ProcessLinkedActorCollisions         15cb0
    ChangeLinkedActors                 15bcc

UpdateIndividualDebris                 145fa
  RemoveProjectile                     1452a
  RemoveMovingTile                     1458d

NewGame                                12efd
  InitializeScoreDisplay               12e8d
```

The hierarchy is substantially better supported than any identifier in it. Do not flatten captures into invented explicit parameters and present that as original Pascal form.

Other likely constructs:

- **`var` records / typed pointers:** `UpdateActor` takes one far record address at `[BP+4/+6]` and returns with `RET 4` (`0x16061`, `0x17780`). `InitializeAnimation(var A; First,Last,Delay,Mode: Byte)` is a natural reconstruction of `0x106ab`, whose four byte parameters still occupy two-byte stack slots. Those slots do not make the source parameters `Word`.
- **`with` statements or local typed record pointers:** repeated computed far-address temporaries, e.g. `[BP-0x3a/-0x38]` in `0x16053`, are consistent with a source `with` selector, but cannot prove that syntax over a temporary pointer variable.
- **`for`, `case`, and `Inc/Dec`:** the bounded counter loops and discriminator branches support idiomatic Pascal equivalents; exact original surface syntax is not unique.
- **Raw file I/O:** calls corresponding to `Assign`, `Reset(F,1)`, `Rewrite(F,1)`, `BlockRead`, `BlockWrite`, `Close`, and explicit `IOResult` are identifiable from operations and call shape. File-record size 1 makes the read counts byte counts; this is not evidence for `file of TActor38`.
- **Assembly/inline code:** register-level map loops, `REP MOVSB`, hardware port I/O, and overflow-flag motion integration strongly suggest assembly sections or routines. The EXE does not distinguish source `asm`, `inline(...)`, and linked assembler object code everywhere.
- **Interrupt procedures:** raw timer handler `0x10fbe` saves AX/BX/CX/DX/SI/DI/DS/ES, executes `ENTER 4,0`, loads DS, and exits by restoration plus `IRET` at `0x11090`. Keyboard handler `0x11091` has corresponding saves/DS setup and `IRET` at `0x11155`. These fit Pascal `interrupt` routines, although handwritten equivalent prologues are possible. Their vector installation is `0x12783..0x127b4`; they are not separate entries in the default 215-function discovery inventory.

## 9. Compiler fingerprint and runtime/support units

### Positive fingerprint, without an exact release claim

The literal `Portions Copyright (c) 1983,91 Borland` occupies EXE offset `0x9b89` (linear `0x19419`). Together with shortstrings, hidden static links, callee-cleaned near/far calls, standard file records and runtime behavior, it strongly supports a **16-bit Borland Turbo Pascal/Pascal-family toolchain**. The banner is a runtime copyright range, **not proof of the compiler's release number, compilation date, or source date**.

Main-module `LEAVE`, immediate `PUSH`, and immediate-multiplier `IMUL` are consistent with the Borland extended-instruction code-generation option; e.g. `0x13006`, `0x12e8b`, `0x12dc3`. The manual describes that option, but those instructions do not identify one release [M1, p. 266]. Many prologues call the stack guard at `0x196df`, consistent with stack checking in those routines; it does not prove one global setting across all units. Exact compiler version, patch level, linker options, `{$D}`/`{$L}` settings and original `uses` clause remain unestablished.

### Likely unit boundaries and roles

Segment names `CODE_n` and function labels such as `FUN_184a_030f` are REA/Ghidra identifiers, not original Pascal names.

| REA linear segment region | Evidence | Likely source/unit attribution |
|---|---|---|
| `10000..182cf` | Main entry `17783`, actors, resources, gameplay, IRQ code | Main program module; exact program name unknown. |
| `182d0..183ef` | Decoder `182d0`, empty initialization-like far routine `183e1` | Custom decompression support unit, original unit name unknown. |
| `183f0..1849f` | `183f0` sets DTA and invokes DOS find-first; `1846b` invokes INT 21h/AH=35h; `18483` invokes AH=25h | Strong **Dos-unit-compatible** fingerprint, including FindFirst/GetIntVec/SetIntVec operations. |
| `184a0..18abf` | Initialization attaches standard text objects (`184a0`); Delay-like `1873c`; speaker disable `18796`; keyboard test/read `1879d/187af` | Strong **Crt-unit-compatible** fingerprint. Not every entry has been matched to a named library symbol. |
| `18ac0..191ff` | VGA DAC writes `18ac0`, custom rendering `18bb4`, sprite creation `18fd7`, sprite resource loader `1909c`, initialization `191ed` | Custom graphics/sprite support; do not label this the standard BGI `Graph` unit without further evidence. |
| `19200..1aa1f` | DS/startup `19200`; stack guard `196df`; I/O status `196a2`; Move-like `19b0e`; signed long division `19b45`; shortstring copy `19bf4`; file I/O `1a7ca..1a948` | Strong **System-runtime-compatible** fingerprint. |

Entry `0x17783..0x17792` calls startup `19200`, graphics initializer `191ed`, CRT-like initializer `184a0`, and decompressor initializer `183e1`, then enters main's own frame. That is consistent with unit initialization, but smart linking and dependency ordering prevent recovering the exact textual `uses` list merely from these four calls.

No object-method/VMT evidence was established for the actor records; there is no basis here to prefer Pascal `object` over `record`.

### Embedded string table: geometry, not a text dump

At `DS:008a`, all 26 consecutive 256-byte blocks fit within initialized data, ending at `DS:1a8a`, EXE interval `[0xb21a,0xcc1a)`. Per-slot counted-string lengths were inspected without emitting their text. Caller `0x12c40..0x12c49` multiplies language by `0x0d00`; `0x12265..0x1226f` also scales a string index by 256. Language is initialized to 0 at `0x12767` and toggled with XOR 1 at `0x12459..0x1245e`.

This supports the proposed storage view `array[0..1,0..12] of string[255]`. The original identifiers, exact lower bounds/enumerated index types, and whether the author used nested arrays or another equivalent declaration are unknown. No personal/contact text was reproduced.

## 10. Actual debug/source metadata check

Measured checks on the complete EXE and supplied companion directory:

- MZ declared end equals physical EOF: `(103-1)*512 + 160 = 52384`. There is **no appended byte region beyond the declared DOS executable**.
- Relocation table ends at file offset `0x76c`; the remaining four header bytes through `0x76f` are zero.
- Case-insensitive byte searches found no `.PAS`, `.TPU`, `.MAP`, `.TDS`, `.SYM`, `.DBG`, `.ASM`, or `.OBJ` strings.
- No matching source/debug companion extensions were present in `original/game`.
- Additional simple signature probes for `TD32`, `NB02`, `NB09`, and byte sequences `FB 09` / `FB 02` found no hits. These probes are **not** a complete parser for every historical Borland debug format.
- Last 16 bytes are `00000200000000000000000000000000`; there is no self-evident source/symbol filename footer.

**Conclusion:** no actual original procedure names, Pascal source filenames, unit names, line tables or recoverable source-text metadata were identified by this inspection. This is a bounded negative result, not proof that debug data of every possible format is absent, and not proof that original source cannot exist elsewhere. Lack of appended metadata cannot establish which compiler debug switches were used; Borland distinguishes unit debug information from standalone debugging output [M1, pp. 264, 267].

## 11. Verification and remaining uncertainty

Performed:

- Recomputed the input EXE SHA-256 and MZ geometry.
- Inspected selected REA assembly, not just pseudocode; independently decoded original relocated bytes for DS initialization, Move, actor access/word stores, signed score comparison, timer handling and debris overflow integration.
- Checked field-width sums and declared offsets for all 16 proposed packed record types: **7, 38, 4, 8, 11, 15, 13, 45, 16, 30, 7, 14, 6, 3, 8, 128** bytes, in declaration order.
- Checked array extents by arithmetic: 32 actors end at `DS:2048`, 1,600 small records at `DS:655e`, 250 group records at `DS:74c6`, seven score rows at `DS:1b5f` (exclusive).
- Framed `GRAN.MST` exactly to EOF following the assembly's read sequence; preserved rather than discarded actor byte `05`.
- Checked embedded string-table bounds without dumping string contents or high-score names.

Not performed during the original research phase: Pascal compilation, changes to `pascal/LZCORE.PAS`, gameplay execution, differential tests of these new declarations, validation of original source identifiers, complete decompilation of every path, or release-specific matching against multiple authentic Borland runtime libraries.

Highest-priority unresolved items are actor byte `05`, kind-dependent actor parameters, spawner bytes `04..07`, sound bytes `04..05`, full meaning of several state/flag bytes, and whether the score-display globals have an unobserved trailing field versus inter-variable alignment. Keep every raw byte in serialization and whole-record copies. Proposed semantic names must not silently become claims about original source.

## Evidence sources

### Local primary sources

- `original/game/LEZAC.EXE`, fingerprint above; MZ header, raw relocated instructions and bounded metadata scans.
- `original/game/GRAN.MST`, fingerprint in section 6; exact read-sequence framing and unknown-byte preservation evidence.
- `analysis/rea/verified/functions/<entry>.json`, specifically the instruction addresses cited throughout. Important entries: `1056b`, `10630`, `106ab`, `108a5`, `10c33`, `111b0`, `11239`, `11287`, `116ca`, `11845`, `12597`, `126e8`, `12adc`, `12e8d`, `12efd`, `12f9f`, `130a3`, `13358`, `1370e`, `13eda`, `13fa6`, `1432a`, `1452a`, `1458d`, `145fa`, `1508b`, `15102`, `15740`, `15872`, `15999`, `15a75`, `15afd`, `15b86`, `15bcc`, `15cb0`, `16053`, `17783`, `183f0`, `1846b`, `18483`, `184a0`, `1873c`, `18796`, `1879d`, `187af`, `18ac0`, `18fd7`, `1909c`, `19200`, `196a2`, `196df`, `19b0e`, `19b45`, `19bf4`, `1a7ca`, `1a7f8`, `1a801`, `1a879`, `1a8e3`, `1a8ea`.

### Existing local secondary navigation

`docs/recovered-routines.md`, `docs/resource-formats.md`, and `docs/analysis.md` were used as navigation and checked against original instructions. The signed high-score comparison and the composite `GRAN.MST` framing are additional findings beyond those documents; no existing documentation was edited.

### Historical primary language reference

**M1:** Borland, *Turbo Pascal Version 6.0 Programmer's Guide*, archived manual. Used for interpreting a compatible dialect/ABI, **not to identify the target as version 6.0**. Relevant printed pages: 28 (packing), 218–223 (storage), 242–245 (parameter/static-link convention), 262–267 (alignment, debug and code generation). The retrieved OCR supplied the quoted page headings and technical descriptions; this report paraphrases rather than reproduces the manual.

```text
PDF archive:
https://bitsavers.org/pdf/borland/turbo_pascal/Turbo_Pascal_Version_6.0_Programmers_Guide.pdf
Retrieved OCR:
https://archive.org/download/bitsavers_borlandturVersion6.0ProgrammersGuide_11460728/Turbo_Pascal_Version_6.0_Programmers_Guide_djvu.txt
```

The configured web extraction helper was unavailable for page extraction; the historical manual's OCR was successfully retrieved directly over HTTPS instead. No compiler-version claim depends on a search snippet.
