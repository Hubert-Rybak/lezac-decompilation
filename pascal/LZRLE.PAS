unit LZRLE;
{ Reconstructed Pascal from REA 0x182d0 (original 082D:0000).
  Names are inferred. This deliberately retains the ORIGINAL UNSAFE behavior.
  It is not the bounded decoder in tools/asset_formats.py. }
{$IFDEF FPC}{$MODE TP}{$ENDIF}
{$R-}{$Q-}

interface

type
  TByteWindow = array[0..65534] of Byte;
  PByteWindow = ^TByteWindow;

procedure LegacyExpandUnsafe(var Source: PByteWindow; PackedLength: Word;
                             var Destination: PByteWindow; Required: Word);

implementation

procedure LegacyExpandUnsafe(var Source: PByteWindow; PackedLength: Word;
                             var Destination: PByteWindow; Required: Word);
var
  InputPos, OutputPos, I, Upper: Word;
  Control, First, Second, RunLength: Byte;
begin
  InputPos := 0;
  OutputPos := 0;
  repeat
    Control := Source^[InputPos];
    First := Source^[Word(InputPos + 1)];
    Second := Source^[Word(InputPos + 2)];
    Inc(InputPos, 3);
    RunLength := (Control shr 4) + 1;
    Upper := Word(OutputPos + RunLength);
    { Inclusive endpoint is observable in the original instructions. }
    for I := OutputPos to Upper do
      Destination^[I] := First;
    Inc(OutputPos, RunLength);
    if OutputPos < Required then
    begin
      RunLength := (Control mod 16) + 1;
      Upper := Word(OutputPos + RunLength);
      for I := OutputPos to Upper do
        Destination^[I] := Second;
      Inc(OutputPos, RunLength)
    end
  until OutputPos >= Required
  { PackedLength is genuinely UNUSED by the original routine. Even Required=0
    executes the first run. Call only inside an adequately sized test arena. }
end;

end.
