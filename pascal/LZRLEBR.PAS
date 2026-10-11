library LZRLEBR;
{ Modern host-verification adapter, NOT original game source or DOS ABI. }
{$mode tp}
uses LZRLE;

procedure lezac_legacy_rle(Input: Pointer; PackedLength: Word;
                          Output: Pointer; Required: Word); cdecl;
var
  Source, Destination: PByteWindow;
begin
  Source := PByteWindow(Input);
  Destination := PByteWindow(Output);
  LegacyExpandUnsafe(Source, PackedLength, Destination, Required)
end;

exports lezac_legacy_rle name 'lezac_legacy_rle';
begin
end.
