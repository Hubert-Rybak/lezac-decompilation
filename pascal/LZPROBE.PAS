program LZPROBE;
{ NEW verification program, not the original main program or a playable game.
  Its output is compared with actual original instructions by test_pascal_probe. }
{$IFDEF FPC}{$MODE TP}{$ENDIF}
{$R-}{$Q-}
uses LZCORE, LZRLE;

const
  HexDigits = '0123456789ABCDEF';
  Samples: array[0..6] of Byte = (1, 2, 3, 4, 255, 0, 128);
  Velocities: array[0..4] of Integer = (-32768, -43, 0, 43, 32767);
var
  Accumulator, LoopIndex, Cell: Word;
  I: Integer;
  Velocity: Integer;
  Value, Tile: Byte;
  Animation: TAnimState;
  Display: TInitState45;
  Collection: TCollectionState;
  ScoreValues, BonusValues: TWordTable12;
  SourceBytes, DestinationBytes: array[0..63] of Byte;
  SourcePointer, DestinationPointer: PByteWindow;

procedure DumpBytes(var Buffer; Count: Word);
var
  P: PByteWindow;
  J: Word;
  B: Byte;
begin
  P := @Buffer;
  for J := 1 to Count do
  begin
    B := P^[J - 1];
    Write(HexDigits[(B shr 4) + 1], HexDigits[(B and 15) + 1])
  end;
  WriteLn
end;

begin
  WriteLn('TYPES ', SizeOf(Integer), ' ', SizeOf(Word), ' ',
          SizeOf(LongInt), ' ', SizeOf(Pointer));
  Accumulator := $FFFF;
  AlternatingChecksum(Samples, Accumulator, SizeOf(Samples));
  Write('CHECKSUM:'); DumpBytes(Accumulator, SizeOf(Accumulator));
  AnimationInit(Animation, 250, 7, 255, 3);
  Write('ANIMATION:'); DumpBytes(Animation, SizeOf(Animation));
  Value := 250;
  WrappingAddClamp(Value, 10, 100);
  Write('WRAP:'); DumpBytes(Value, SizeOf(Value));
  for I := 0 to 4 do
  begin
    Velocity := FrictionStep(Velocities[I]);
    Write('FRICTION', I, ':'); DumpBytes(Velocity, SizeOf(Velocity))
  end;
  FillChar(Display, SizeOf(Display), $A5);
  LoopIndex := $BEEF;
  State45Init(Display, $FF00, LoopIndex);
  Write('STATE45:'); DumpBytes(Display, SizeOf(Display));
  Write('INDEX:'); DumpBytes(LoopIndex, SizeOf(LoopIndex));
  for I := 0 to 11 do
  begin
    ScoreValues[I] := 100 + I;
    BonusValues[I] := 200 + I
  end;
  Tile := $67;
  Cell := $8001;
  Collection.Score := 65530;
  Collection.Bonus := $BEEF;
  Collection.CollectedTile := $A5;
  Collection.AbovePresent := $5A;
  CollectTile(Tile, Cell, $7FFF, ScoreValues, BonusValues, Collection);
  Write('COLLECTION:'); DumpBytes(Collection, SizeOf(Collection));
  Write('TILE:'); DumpBytes(Tile, SizeOf(Tile));
  Write('CELL:'); DumpBytes(Cell, SizeOf(Cell));
  FillChar(SourceBytes, SizeOf(SourceBytes), 0);
  SourceBytes[0] := $10;
  SourceBytes[1] := Ord('A');
  SourceBytes[2] := Ord('B');
  SourcePointer := @SourceBytes;
  DestinationPointer := @DestinationBytes;
  FillChar(DestinationBytes, SizeOf(DestinationBytes), $CD);
  LegacyExpandUnsafe(SourcePointer, 3, DestinationPointer, 3);
  Write('RLE:'); DumpBytes(DestinationBytes, 16);
  FillChar(DestinationBytes, SizeOf(DestinationBytes), $CD);
  LegacyExpandUnsafe(SourcePointer, 3, DestinationPointer, 0);
  Write('RLEZERO:'); DumpBytes(DestinationBytes, 16)
end.
