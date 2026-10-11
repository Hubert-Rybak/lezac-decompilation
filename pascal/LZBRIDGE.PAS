library LZBRIDGE;

{ MODERN HOST VERIFICATION SCAFFOLDING ONLY.
  Free Pascal shared-library exports adapt the existing ctypes/C-ABI test
  signatures to LZCORE. This is not a reconstructed DOS library or TP ABI.
  No game arithmetic is implemented in this bridge. }

{$MODE TP}
{$R-}
{$Q-}

uses LZCORE;

type
  PByteValue = ^Byte;
  PWordValue = ^Word;
  PAnimState = ^TAnimState;
  PInitState45 = ^TInitState45;
  PWordTable12 = ^TWordTable12;
  PCollectionState = ^TCollectionState;

function lezac_alternating_checksum(Seed: Word; Bytes: Pointer;
                                    Count: Word): Word; cdecl;
var
  EmptySource: Byte;
begin
  { Still exercise the core's empty-loop path, even for a nil host pointer. }
  if Count = 0 then
  begin
    EmptySource := 0;
    AlternatingChecksum(EmptySource, Seed, Count);
  end
  else
    AlternatingChecksum(PByteValue(Bytes)^, Seed, Count);
  lezac_alternating_checksum := Seed;
end;

procedure lezac_animation_init(State: PAnimState;
                               StartFrame, EndFrame, Delay, Mode: Byte); cdecl;
begin
  AnimationInit(State^, StartFrame, EndFrame, Delay, Mode);
end;

function lezac_wrapping_add_clamp(Value, Delta, Limit: Byte): Byte; cdecl;
begin
  WrappingAddClamp(Value, Delta, Limit);
  lezac_wrapping_add_clamp := Value;
end;

function lezac_friction_step(Velocity: Integer): Integer; cdecl;
begin
  lezac_friction_step := FrictionStep(Velocity);
end;

procedure lezac_state45_init(State: PInitState45; FillWord: Word;
                             LoopIndex: PWordValue); cdecl;
begin
  State45Init(State^, FillWord, LoopIndex^);
end;

procedure lezac_collect_tile(Tile: PByteValue; Cell: PWordValue;
                             AboveCell: Word;
                             ScoreValues, BonusValues: PWordTable12;
                             State: PCollectionState); cdecl;
begin
  CollectTile(Tile^, Cell^, AboveCell, ScoreValues^, BonusValues^, State^);
end;

exports
  lezac_alternating_checksum,
  lezac_animation_init,
  lezac_wrapping_add_clamp,
  lezac_friction_step,
  lezac_state45_init,
  lezac_collect_tile;

begin
end.
