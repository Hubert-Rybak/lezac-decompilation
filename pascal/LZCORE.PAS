unit LZCORE;

{ Hand reconstruction from the six original instruction bodies, not the
  author's source or an assertion of byte-identical Borland compiler output.
  Names are descriptive. See docs/pascal-core.md for evidence and adapters. }

{$IFDEF FPC}
  {$MODE TP}
{$ENDIF}
{$R-} { Byte/Word assignments deliberately wrap before later comparisons. }
{$Q-}

interface

type
  TAnimState = packed record
    Frame, StartFrame, EndFrame, Counter, Delay, Mode: Byte;
    Step: ShortInt;
  end;

  { Only the written 45-byte region is known, not an enclosing record stride. }
  TInitState45 = packed record
    Word0, Word2: Word;
    First, Second: array[0..9] of Word;
    Mode: Byte;
  end;

  TWordTable12 = array[0..11] of Word;

  { Adapter grouping of four separate original globals, NOT an original
    contiguous record: DS:2074, DS:2072, DS:79A3, DS:661E respectively. }
  TCollectionState = packed record
    Score, Bonus: Word;
    CollectedTile, AbovePresent: Byte;
  end;

{ These first three declarations keep the source-level parameter order
  indicated by the original callee-cleaned Pascal stack arguments. }
procedure AlternatingChecksum(var Source; var Accumulator: Word; Count: Word);
procedure AnimationInit(var State: TAnimState;
                        StartFrame, EndFrame, Delay, Mode: Byte);
procedure WrappingAddClamp(var Value: Byte; Delta, Limit: Byte);

{ These facades supply only the missing parent/global context. The recovered
  operations remain nested procedures below; their parent is not the whole
  original actor routine. The function result is a convenience, not an
  original return value. }
function FrictionStep(Velocity: Integer): Integer;
procedure State45Init(var State: TInitState45; FillWord: Word;
                      var LoopIndex: Word);
procedure CollectTile(var Tile: Byte; var Cell: Word; AboveCell: Word;
                      var ScoreValues, BonusValues: TWordTable12;
                      var State: TCollectionState);

implementation

type
  { An unchecked view of an untyped buffer, not a recovered array bound.
    R- is intentional; the caller supplies Count valid bytes (0..65535). }
  TByteView = array[0..0] of Byte;
  PByteView = ^TByteView;

procedure AlternatingChecksum(var Source; var Accumulator: Word; Count: Word);
var
  I, Sample: Word;
  Bytes: PByteView;
begin
  Bytes := @Source;
  for I := 1 to Count do
  begin
    Sample := Bytes^[I - 1];
    if Odd(I) then
      Accumulator := Accumulator + Sample
    else
      Accumulator := Accumulator - Sample;
  end;
end;

procedure AnimationInit(var State: TAnimState;
                        StartFrame, EndFrame, Delay, Mode: Byte);
begin
  { Keep the observed store order 0,1,2,4,5,6,3. }
  State.Frame := StartFrame;
  State.StartFrame := StartFrame;
  State.EndFrame := EndFrame;
  State.Delay := Delay;
  State.Mode := Mode;
  State.Step := 1;
  State.Counter := Delay;
end;

procedure WrappingAddClamp(var Value: Byte; Delta, Limit: Byte);
begin
  { The byte store MUST precede the comparison: 250 + 10 becomes 4. }
  Value := Value + Delta;
  if Value > Limit then
    Value := Limit;
end;

function FrictionStep(Velocity: Integer): Integer;
var
  LocalVelocity: Integer;

  procedure ApplyFriction;
  begin
    { 15B86 uses a static link to a signed word in its parent's frame.
      Narrow Abs explicitly: a host compiler may widen its expression.
      Integer($8000) remains negative, just as the original CWD/XOR/SUB. }
    if Integer(Abs(LocalVelocity)) < 43 then
      LocalVelocity := 0
    else if LocalVelocity < 0 then
      Inc(LocalVelocity, 42)
    else
      Dec(LocalVelocity, 42);
  end;

begin
  LocalVelocity := Velocity;
  ApplyFriction;
  FrictionStep := LocalVelocity;
end;

procedure State45Init(var State: TInitState45; FillWord: Word;
                      var LoopIndex: Word);

  procedure Initialize(var Target: TInitState45);
  begin
    { 12E8D has one record var argument plus a hidden static link. FillWord
      and LoopIndex stand for DS:2070 and DS:2082, not original parameters.
      Do not rely on a compiler's unspecified post-FOR control-variable value. }
    LoopIndex := 0;
    while True do
    begin
      Target.First[LoopIndex] := FillWord;
      Target.Second[LoopIndex] := FillWord;
      if LoopIndex = 9 then
        Break;
      Inc(LoopIndex);
    end;
    Target.Mode := 2;
    Target.Word0 := 0;
    Target.Word2 := 0;
  end;

begin
  Initialize(State);
end;

procedure CollectTile(var Tile: Byte; var Cell: Word; AboveCell: Word;
                      var ScoreValues, BonusValues: TWordTable12;
                      var State: TCollectionState);

  procedure Collect;
  var
    K: Byte;
    CurrentWord, PreviousWord: Word;
  begin
    { 15AFD has no explicit parameters, only a hidden static link. These
      enclosing values replace its scattered globals and selected map cells. }
    State.CollectedTile := 0;
    State.Bonus := 0;
    State.AbovePresent := 0;
    if (ShortInt(Tile) > $66) and (ShortInt(Tile) < $73) then
    begin
      State.CollectedTile := Tile;
      K := Tile - $67;
      State.Bonus := BonusValues[K];
      State.Score := State.Score + ScoreValues[K];
      CurrentWord := Cell;
      PreviousWord := AboveCell;
      if (PreviousWord <> 0) and (PreviousWord < $8000) then
        State.AbovePresent := 1;
      if CurrentWord < $8000 then
      begin
        Cell := 0;
        Tile := 0;
      end
      else
        Tile := $FF;
    end;
  end;

begin
  Collect;
end;

end.
