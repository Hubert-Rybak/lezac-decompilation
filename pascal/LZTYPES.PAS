unit LZTYPES;
{ Evidence-backed storage views, NOT original author identifiers/declarations.
  Unknown bytes are real storage and MUST be preserved. See docs/pascal-layouts.md.
  TFile128 describes the original Borland runtime, NOT the native FPC file type.
  Array bounds below are measured storage views, not recovered source bounds. }
{$IFDEF FPC}{$MODE TP}{$ENDIF}
interface

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

type
  TActorStorageView = array[0..31] of TActor38;
  TSmallMotionStorage = array[1..1600] of TDebris11;
  TGroupStorage = array[1..250] of TGroup15;
  THighScoreStorage = array[1..7] of TScore13;
  TLanguageTextView = array[0..1, 0..12] of string[255];

implementation
end.
