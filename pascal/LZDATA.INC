{ LZDATA.INC -- typed constants and global variables of the main program.

  Declared in the original data-segment order. The comments give the original
  DS offsets (load image of LEZAC.EXE, SHA-256 7579...53ec). Names are
  descriptive choices, not recovered identifiers. Types follow the original
  instructions (signed/unsigned compares, CBW/CWD extension, byte/word
  stores); unknown or never-referenced storage is kept so that the relative
  layout, and therefore any out-of-bounds behaviour, is preserved. }

const
  { DS:0000 points for the tile codes collected in CollectTile }
  ScoreTable: array[0..12] of Word =
    (0, 50, 100, 200, 250, 500, 800, 1000, 1500, 2000, 3000, 5000, 1000);
  { DS:001A sprite numbers of the twelve bonus tiles }
  BonusSprites: array[0..11] of Word =
    (80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 86);
  { DS:0032 bomb kind carried by each bomb slot }
  KindBomb: array[1..4] of Byte = (1, 2, 3, 4);
  { DS:0036 points of the seven pick-up kinds }
  PickupScore: array[1..7] of Word = (2000, 1000, 1500, 2000, 3000, 1000, 5000);
  { DS:0044 sprite of the seven pick-up kinds }
  PickupSprite: array[1..7] of Word = (88, 86, 87, 88, 89, 86, 90);
  { DS:0052 cumulative percentages for choosing a pick-up kind }
  PickupChance: array[0..7] of Byte = (40, 65, 71, 78, 83, 89, 93, 100);
  { DS:005A first/last frame of the 24 animation sequences }
  Anims: array[1..24, 1..2] of Byte =
    ((44, 45), (46, 47), (1, 2), (34, 39), (2, 9), (10, 17), (21, 28),
     (29, 35), (69, 73), (74, 79), (40, 42), (50, 52), (54, 56), (41, 42),
     (43, 44), (40, 40), (48, 49), (43, 43), (53, 53), (57, 57), (1, 2),
     (11, 11), (12, 12), (13, 13));
  { DS:008A the bilingual text pages; '\' starts a new line. The game's own
    font maps ':' '=' ';' '>' '?' to punctuation. The original declared them
    as string[255] typed constants, which Turbo Pascal pads with zero bytes;
    the start-up checksum covers that padding. Free Pascal pads string
    constants with #0 and spaces instead, so the table is stored as zero-
    padded character arrays that carry their own length byte, and the
    program reads it through the string view Texts declared below. }
  TextStore: array[0..1, 1..13] of array[0..255] of Char = (
{$I LZTEXTS.INC}
  );
  { DS:1A8A offset of each level in LEVELS.DAT; levels 8 and 9 are absent }
  LevelOffsets: array[1..9] of LongInt =
    (0, 1242, 4923, 10984, 15225, 21546, 36128, 0, 0);
  { DS:1AAE..1ABD palette flash colours }
  FlashDark: TRGB3 = (Red: 1; Green: 1; Blue: 41);
  FlashWhite: TRGB3 = (Red: 63; Green: 63; Blue: 63);
  FlashRed: TRGB3 = (Red: 40; Green: 20; Blue: 30);
  FlashBlack: TRGB3 = (Red: 0; Green: 0; Blue: 0);
  { DS:1ABE }
  PlayerDigits: string[2] = '12';

var
  Texts: array[0..1, 1..13] of string[255] absolute TextStore;  { DS:008A }
  HighScores: array[1..7] of TScore13;          { DS:1B04 }
  OldInt9: Pointer;                             { DS:1B60 }
  OldInt1C: Pointer;                            { DS:1B64 }
  Reserved1B68: LongInt;                        { DS:1B68 never referenced }
  Bombs: array[1..2, 1..4] of Byte;             { DS:1B6C bombs per player/kind }
  SelBomb: array[1..2] of Byte;                 { DS:1B74 selected bomb slot }
  BombIconDirty: array[1..2] of Byte;           { DS:1B76 }
  { DS:1B78 key-down flags set by the keyboard handler:
    1 M, 2 Z, 3 X, 4 N, 5 C, 6 Up, 7 Left, 8 Right, 9 Ins/KP0, 10 Down }
  Keys: array[1..10] of Byte;
  CurKeys: array[1..5] of Byte;                 { DS:1B82 jump, left, right, fire, down }
  Actors: array[1..32] of TActor38;             { DS:1B88 }
  Around: array[0..15] of Byte;                 { DS:2048 }
  LastKey: Char;                                { DS:2058 }
  GroupCols: Word;                              { DS:205A }
  GroupRows: Word;                              { DS:205C }
  GroupTag: Word;                               { DS:205E }
  ScanStart: Word;                              { DS:2060 }
  ScanStep: Word;                               { DS:2062 }
  RowStep: Word;                                { DS:2064 }
  RowBytes: Word;                               { DS:2066 }
  GroupLeft: Word;                              { DS:2068 }
  GroupTop: Integer;                            { DS:206A }
  GroupBottom: Integer;                         { DS:206C }
  MapBSeg: Word;                                { DS:206E }
  DigitBase: Word;                              { DS:2070 }
  TmpA: Word;                                   { DS:2072 shared scratch }
  TmpB: Word;                                   { DS:2074 shared scratch }
  ProjLast: Word;                               { DS:2076 }
  HitCount: Word;                               { DS:2078 }
  GroupBase: Word;                              { DS:207A }
  DebrisBase: Word;                             { DS:207C }
  DebrisLast: Word;                             { DS:207E }
  GroupLast: Word;                              { DS:2080 }
  GI: Word;                                     { DS:2082 shared index }
  ScoreGap: Word;                               { DS:2084 Ofs(ScoreDisp2) - Ofs(ScoreDisp1) }
  BonusRequired: Word;                          { DS:2086 }
  BonusCollected: Word;                         { DS:2088 }
  BonusShown: Word;                             { DS:208A }
  Flag: Byte;                                   { DS:208C }
  ExtraCount: Byte;                             { DS:208D }
  KindTenCount: Byte;                           { DS:208E }
  CellStep: Integer;                            { DS:2090 }
  CellStep2: Integer;                           { DS:2092 }
  CamMaxX: Integer;                             { DS:2094 }
  CamMaxY: Integer;                             { DS:2096 }
  ShakeOffset: Integer;                         { DS:2098 }
  ShakeTimer: Integer;                          { DS:209A }
  Shaking: Integer;                             { DS:209C }
  Debris: array[1..1600] of TDebris11;          { DS:209E }
  HitCells: array[1..30] of Word;               { DS:655E }
  HitOffsets: array[1..30] of Word;             { DS:659A }
  HitRefs: array[1..30] of Word;                { DS:65D6 }
  MapB: Pointer;                                { DS:6612 }
  MapBAlloc: Pointer;                           { DS:6616 }
  MapAAlloc: Pointer;                           { DS:661A }
  ByteTemp2: Shortint;                          { DS:661E }
  Groups: array[1..250] of TGroup15;            { DS:6620 }
  Spawns: array[1..20] of TSpawn30;             { DS:74C6 }
  MapPoints: array[1..25] of TMapPoint7;        { DS:771E }
  Replacements: array[1..10] of TTileReplacement14; { DS:77CE }
  ScoreDisp1: TScoreDisplay45;                  { DS:785A }
  ScoreDisp2: TScoreDisplay45;                  { DS:7888 }
  ScorePtr: Pointer;                            { DS:78B6 }
  LevelSize: Word;                              { DS:78BA }
  ViewCenterX: Word;                            { DS:78BC }
  ViewCells: Word;                              { DS:78BE }
  NoteIndex: Word;                              { DS:78C0 }
  FrameCount: Word;                             { DS:78C2 }
  NextTag: Word;                                { DS:78C4 }
  BuildingCells: Word;                          { DS:78C6 }
  Destroyed: Word;                              { DS:78C8 }
  DelayStep: Word;                              { DS:78CA }
  FrameDelay: Word;                             { DS:78CC }
  TypeDelay: Word;                              { DS:78CE }
  StartDelay: Word;                             { DS:78D0 }
  MoveVX: Shortint;                             { DS:78D2 }
  MovePX: Byte;                                 { DS:78D3 }
  MoveVY: Shortint;                             { DS:78D4 }
  MovePY: Byte;                                 { DS:78D5 }
  ProjWeight: array[1..200] of Byte;             { DS:78D6 }
  SoundPriority: Shortint;                      { DS:799E }
  SoundRequest: Shortint;                       { DS:799F }
  SoundTick: Byte;                              { DS:79A0 }
  SoundGate: Byte;                              { DS:79A1 }
  SoundStep: Byte;                              { DS:79A2 }
  ByteTemp: Byte;                               { DS:79A3 shared scratch }
  Minute1: Byte;                                { DS:79A4 }
  PointCount: Byte;                             { DS:79A5 }
  SpawnCount: Byte;                             { DS:79A6 }
  ReplCount: Byte;                              { DS:79A7 }
  Second1: Byte;                                { DS:79A8 }
  Hundredth1: Byte;                             { DS:79A9 }
  Minute2: Byte;                                { DS:79AA }
  Second2: Byte;                                { DS:79AB }
  Hundredth2: Byte;                             { DS:79AC }
  FireLevel: Byte;                              { DS:79AD }
  Pickup: array[1..2] of Byte;                  { DS:79AE }
  BombSwitch: array[1..2] of Byte;              { DS:79B0 }
  BonusRain: Byte;                              { DS:79B2 }
  TargetPercent: Byte;                          { DS:79B3 }
  CountedTile: Byte;                            { DS:79B4 }
  Percent: Byte;                                { DS:79B5 }
  ShownPercent: Byte;                           { DS:79B6 }
  Level: Byte;                                  { DS:79B7 }
  Players: Byte;                                { DS:79B8 }
  AllDeadTimer: Byte;                           { DS:79B9 }
  TypingKey: Byte;                                { DS:79BA }
  BombSwitchPtr: Pointer;                       { DS:79BC }
  Sounds: Pointer;                              { DS:79C0 }
  SoundOn: Byte;                                { DS:79C4 }
  BonusDone: Byte;                              { DS:79C5 }
  DestructionDone: Byte;                        { DS:79C6 }
  Unknown79C7: Byte;                            { DS:79C7 written, never read }
  MotionOk: Byte;                               { DS:79C8 }
  SavedBreak: Boolean;                          { DS:79C9 }
  CanContinue: Byte;                            { DS:79CA }
  FlashCount: Byte;                             { DS:79CB }
  Language: Byte;                               { DS:79CC }
  FlashColour: array[1..3] of Byte;             { DS:79CE }
  FlashNow: array[1..3] of TRGB3;               { DS:79D2 }
  FlashGoal: array[1..3] of TRGB3;              { DS:79DC }
  PlayerState: array[1..2] of Byte;             { DS:79E6 }
  Touches: array[1..2] of Byte;                 { DS:79E8 }
  Lives: array[1..2] of Byte;                   { DS:79EA }
  ShownEnergy: array[1..2] of Byte;             { DS:79EC }
  ViewTilesSaved: Word;                         { DS:79EE }
  ViewOfs: array[1..2] of Word;                 { DS:79F0 }
  HudX: array[1..2] of Word;                    { DS:79F4 }
  SpriteSet: Byte;                              { DS:79F8 }
  MotionCount: Byte;                            { DS:79F9 }
  Motions: array[1..30] of TMotion16;           { DS:79FA }
  SinTable: array[0..127] of TReal;             { DS:7BDA }
  LevelFile: file;                              { DS:7EDA }
