unit LZTPRT;
{ Turbo Pascal 6 runtime behaviour that the game depends on and that the Free
  Pascal runtime does not reproduce: the 6-byte software Real arithmetic, the
  Random/Randomize generator and the Crt Sound/NoSound PIT programming.

  Crt KeyPressed/ReadKey are included as well: Free Pascal's Crt switches to
  the enhanced INT 16h functions (10h/11h) on an enhanced keyboard and so
  also returns keys such as F11/F12, which Turbo Pascal's standard functions
  (00h/01h) never deliver.

  This is a RUNTIME SUBSTITUTION, not recovered game logic. Each routine is
  written in Pascal from the algorithm of the matching routine in the original
  executable's System/Crt code (REA linear addresses in the comments). The
  "register" variables mirror that code so that rounding, truncation and
  overflow behave identically; tests/test_tp_runtime.py compares them with the
  original instructions. Names are descriptive, not Borland identifiers. }
{$IFDEF FPC}{$MODE TP}{$ENDIF}
{$R-}{$Q-}

interface

type
  { The stored 6-byte Real: W0 = exponent byte + lowest mantissa byte,
    W1 = middle mantissa word, W2 = top mantissa word with the sign in bit 15.
    These are the AX, BX and DX words of the original register convention. }
  TReal = packed record
    W0, W1, W2: Word;
  end;

const
  RealZero: TReal = (W0: 0; W1: 0; W2: 0);

var
  { System.RandSeed of the original runtime (initially 0). }
  TPRandSeed: LongInt;

function RFromLong(L: LongInt): TReal;          { 1A04E }
function RAdd(A, B: TReal): TReal;              { 1A0E9 -> 19DE5 }
function RSub(A, B: TReal): TReal;              { 19DE1 }
function RMul(A, B: TReal): TReal;              { 1A0FB -> 19EA8 }
function RDiv(A, B: TReal): TReal;              { 1A101 -> 19FAD }
function RCmp(A, B: TReal): Integer;            { 1A10B -> 1A024: -1, 0, 1 }
function RTrunc(A: TReal): LongInt;             { 1A113 -> 1A08D }
function RRound(A: TReal): LongInt;             { 1A11B -> 1A08D }
function RInt(A: TReal): TReal;                 { 1A15D }
function RFrac(A: TReal): TReal;                { 1A1AE }
function RSqrt(A: TReal): TReal;                { 1A1C2 }
function RSin(A: TReal): TReal;                 { 1A234 }
function RCos(A: TReal): TReal;                 { 1A221 }

{ Flag-returning cores (False where the original sets CF) for verification. }
function RealAdd(var R: TReal; A, B: TReal): Boolean;
function RealMul(var R: TReal; A, B: TReal): Boolean;
function RealDiv(var R: TReal; A, B: TReal): Boolean;
function RealTruncRound(A: TReal; DoRound: Boolean; var Value: LongInt): Boolean;

function TPRandom(Range: Word): Word;           { 1A5A8 }
{$IFDEF MSDOS}
procedure TPRandomize;                          { 1A62F }
procedure TPSound(Hz: Word);                    { 18769 }
procedure TPNoSound;                            { 18796 }
function TPKeyPressed: Boolean;                 { 1879D }
function TPReadKey: Char;                       { 187AF }
{$ENDIF}

implementation

{$IFDEF MSDOS}
var
  { Crt's pending scan code of an extended key (DS:7F6D in the original). }
  TPScanCode: Byte;
{$ENDIF}

{ ---- 16-bit helpers modelling ADD/ADC and SUB/SBB with an explicit carry --- }

function AddC(A, B: Word; var C: Word): Word;
var
  S: LongInt;
begin
  S := LongInt(A) + LongInt(B) + LongInt(C);
  AddC := Word(S and $FFFF);
  C := Word(S shr 16);
end;

function SubB(A, B: Word; var Borrow: Word): Word;
var
  S: LongInt;
begin
  S := LongInt(A) - LongInt(B) - LongInt(Borrow);
  if S < 0 then
  begin
    SubB := Word(S + 65536);
    Borrow := 1;
  end
  else
  begin
    SubB := Word(S);
    Borrow := 0;
  end;
end;

procedure Mul16(A, B: Word; var Lo16, Hi16: Word);
var
  P: LongWord;
begin
  P := LongWord(A) * LongWord(B);
  Lo16 := Word(P and $FFFF);
  Hi16 := Word(P shr 16);
end;

procedure SetLow(var W: Word; B: Byte);
begin
  W := (W and $FF00) or B;
end;

procedure MakeZero(var R: TReal);
begin
  R.W0 := 0;
  R.W1 := 0;
  R.W2 := 0;
end;

procedure Store(var R: TReal; AX, BX, DX: Word);
begin
  R.W0 := AX;
  R.W1 := BX;
  R.W2 := DX;
end;

{ ---- 19DE5: addition. Returns False where the original sets CF. --------- }

function RealAdd(var R: TReal; A, B: TReal): Boolean;
var
  AX, BX, DX, CX, SI, DI, BP, T, C: Word;
  Shift: Byte;
  Differ: Boolean;
begin
  RealAdd := True;
  AX := A.W0; BX := A.W1; DX := A.W2;
  CX := B.W0; SI := B.W1; DI := B.W2;
  if Lo(CX) = 0 then
  begin
    R := A;
    Exit;
  end;
  if Lo(AX) = 0 then
  begin
    R := B;
    Exit;
  end;
  if Lo(AX) > Lo(CX) then
  begin
    T := AX; AX := CX; CX := T;
    T := BX; BX := SI; SI := T;
    T := DX; DX := DI; DI := T;
  end;
  Shift := Lo(CX) - Lo(AX);
  if Shift >= 40 then
  begin
    Store(R, CX, SI, DI);
    Exit;
  end;
  BP := (DX and $8000) or Lo(CX);
  Differ := ((BP xor DI) and $8000) <> 0;
  AX := AX and $FF00;
  DX := DX or $8000;
  DI := DI or $8000;
  while Shift >= 8 do
  begin
    AX := (AX shr 8) or ((BX and $FF) shl 8);
    BX := (BX shr 8) or ((DX and $FF) shl 8);
    DX := DX shr 8;
    Dec(Shift, 8);
  end;
  while Shift > 0 do
  begin
    AX := (AX shr 1) or ((BX and 1) shl 15);
    BX := (BX shr 1) or ((DX and 1) shl 15);
    DX := DX shr 1;
    Dec(Shift);
  end;
  CX := CX and $FF00;
  if not Differ then
  begin
    C := 0;
    AX := AddC(AX, CX, C);
    BX := AddC(BX, SI, C);
    DX := AddC(DX, DI, C);
    CX := BP;
    if C <> 0 then
    begin
      AX := (AX shr 1) or ((BX and 1) shl 15);
      BX := (BX shr 1) or ((DX and 1) shl 15);
      DX := (DX shr 1) or $8000;
      SetLow(CX, Byte(Lo(CX) + 1));
      if Lo(CX) = 0 then
      begin
        RealAdd := False;
        Exit;
      end;
    end;
  end
  else
  begin
    C := 0;
    AX := SubB(AX, CX, C);
    BX := SubB(BX, SI, C);
    DX := SubB(DX, DI, C);
    CX := BP;
    if C <> 0 then
    begin
      DX := not DX;
      BX := not BX;
      if AX = 0 then C := 1 else C := 0;
      AX := Word((65536 - LongInt(AX)) and $FFFF);
      BX := AddC(BX, 0, C);
      DX := AddC(DX, 0, C);
      CX := CX xor $8000;
    end;
    if (DX or BX or AX) = 0 then
    begin
      MakeZero(R);
      Exit;
    end;
    while (DX and $8000) = 0 do
    begin
      DX := (DX shl 1) or (BX shr 15);
      BX := (BX shl 1) or (AX shr 15);
      AX := AX shl 1;
      SetLow(CX, Byte(Lo(CX) - 1));
      if Lo(CX) = 0 then
      begin
        MakeZero(R);
        Exit;
      end;
    end;
  end;
  { 19E52: round half up on the guard byte }
  C := 0;
  AX := AddC(AX, $80, C);
  BX := AddC(BX, 0, C);
  DX := AddC(DX, 0, C);
  if C <> 0 then
  begin
    DX := (DX shr 1) or $8000;
    SetLow(CX, Byte(Lo(CX) + 1));
    if Lo(CX) = 0 then
    begin
      RealAdd := False;
      Exit;
    end;
  end;
  SetLow(AX, Lo(CX));
  DX := (DX and $7FFF) or (CX and $8000);
  Store(R, AX, BX, DX);
end;

{ ---- 19F8B: common rounding/exponent tail of multiplication and division - }

function FinishProduct(var R: TReal; AX, BX, DX, CX: Word): Boolean;
var
  C: Word;
begin
  FinishProduct := True;
  C := 0;
  AX := AddC(AX, $80, C);
  BX := AddC(BX, 0, C);
  DX := AddC(DX, 0, C);
  if C <> 0 then
  begin
    DX := (DX shr 1) or $8000;
    Inc(CX);
  end;
  if (CX and $4000) <> 0 then
  begin
    MakeZero(R);
    Exit;
  end;
  Inc(CX);
  SetLow(AX, Lo(CX));
  DX := DX xor (CX and $FF00);
  if (Hi(CX) and 1) <> 0 then
    FinishProduct := False;
  Store(R, AX, BX, DX);
end;

{ ---- 19EA8: multiplication, including its partial-product truncation ---- }

function RealMul(var R: TReal; A, B: TReal): Boolean;
var
  AX, BX, DX, CX, SI, DI, BP, ExpWord, T, L, H, C: Word;
  M0, M2, M4, M6, M8, MA, AccS, AccD, AccB: Word;
  Product: Byte;
  Short: Boolean;
begin
  AX := A.W0; BX := A.W1; DX := A.W2;
  CX := B.W0; SI := B.W1; DI := B.W2;
  if (Lo(AX) = 0) or (Lo(CX) = 0) then
  begin
    MakeZero(R);
    RealMul := True;
    Exit;
  end;
  BP := DX;
  DX := (DX xor DI) and $8000;
  DX := DX or Lo(AX);
  AX := AX and $FF00;
  T := Lo(DX) + Lo(CX);
  DX := ((Hi(DX) + (T shr 8)) shl 8) or (T and $FF);
  CX := CX and $FF00;
  BP := BP or $8000;
  DI := DI or $8000;
  ExpWord := DX;
  Short := True;
  if (Hi(AX) <> 0) or (BX <> 0) then
  begin
    if (Hi(CX) <> 0) or (SI <> 0) then
      Short := False
    else
    begin
      T := CX; CX := AX; AX := T;
      T := SI; SI := BX; BX := T;
      T := DI; DI := BP; BP := T;
    end;
  end;
  if Short then
  begin
    { 19EDF: one operand has a 16-bit mantissa in BP }
    BX := Word(Hi(BP)) * Hi(CX);
    Mul16(BP, SI, L, H);
    C := 0;
    BX := AddC(BX, L, C);
    DX := H + C;
    CX := DX;
    Mul16(BP, DI, L, H);
    C := 0;
    AX := AddC(L, CX, C);
    DX := H + C;
    { 19F79: XCHG BX,AX }
    T := AX; AX := BX; BX := T;
  end
  else
  begin
    { 19EFE: full product, keeping only the words the original keeps }
    M0 := AX; M2 := BX; M4 := BP; M6 := CX; M8 := SI; MA := DI;
    AccS := Word(Hi(M0)) * Hi(M6);
    AccD := 0;
    AccB := 0;
    Mul16(M0, M8, L, H);
    C := 0; AccS := AddC(AccS, L, C); AccD := AddC(AccD, H, C); AccB := AccB + C;
    Mul16(M2, M6, L, H);
    C := 0; AccS := AddC(AccS, L, C); AccD := AddC(AccD, H, C); AccB := AccB + C;
    AccS := 0;
    Mul16(M0, MA, L, H);
    C := 0; AccD := AddC(AccD, L, C); AccB := AddC(AccB, H, C); AccS := AccS + C;
    Mul16(M2, M8, L, H);
    C := 0; AccD := AddC(AccD, L, C); AccB := AddC(AccB, H, C); AccS := AccS + C;
    Mul16(M4, M6, L, H);
    C := 0; AccD := AddC(AccD, L, C); AccB := AddC(AccB, H, C); AccS := AccS + C;
    AccD := 0;
    Mul16(M2, MA, L, H);
    C := 0; AccB := AddC(AccB, L, C); AccS := AddC(AccS, H, C); AccD := AccD + C;
    Mul16(M4, M8, L, H);
    C := 0; AccB := AddC(AccB, L, C); AccS := AddC(AccS, H, C); AccD := AccD + C;
    Mul16(M4, MA, L, H);
    C := 0;
    AX := AddC(L, AccS, C);
    DX := AddC(H, AccD, C);
    { 19F79: XCHG BX,AX with BX = AccB }
    BX := AX;
    AX := AccB;
  end;
  CX := ExpWord;
  if (DX and $8000) = 0 then
  begin
    DX := (DX shl 1) or (BX shr 15);
    BX := (BX shl 1) or (AX shr 15);
    AX := AX shl 1;
    Dec(CX);
  end;
  CX := Word(LongInt(CX) - $8081);
  RealMul := FinishProduct(R, AX, BX, DX, CX);
end;

{ ---- 19FAD: division by restoring shifts, with an inverted quotient ---- }

function RealDiv(var R: TReal; A, B: TReal): Boolean;
var
  AX, BX, DX, CX, SI, DI, BP, ExpWord, Q, W1, W2, Borrow: Word;
  AH, CH: Byte;
  Count: Integer;
  CF, Below: Boolean;
  Step: Integer;
begin
  AX := A.W0; BX := A.W1; DX := A.W2;
  CX := B.W0; SI := B.W1; DI := B.W2;
  if Lo(AX) = 0 then
  begin
    MakeZero(R);
    RealDiv := True;
    Exit;
  end;
  BP := DX;
  DX := DX xor DI;
  DI := DI or $8000;
  BP := BP or $8000;
  DX := DX and $8000;
  DX := DX or Lo(AX);
  AX := AX and $FF00;
  if Lo(DX) < Lo(CX) then
    DX := ((Hi(DX) - 1) shl 8) or Byte(Lo(DX) - Lo(CX))
  else
    DX := (DX and $FF00) or Byte(Lo(DX) - Lo(CX));
  ExpWord := DX;
  AH := Hi(AX);
  CH := Hi(CX);
  Count := 2;
  Q := 1;
  W1 := 0;
  W2 := 0;
  CF := False;
  Step := 0; { 0 = compare at 19FCE, 1 = shift at 19FE4 }
  while True do
  begin
    if Step = 0 then
    begin
      { 19FCE }
      if BP <> DI then Below := BP < DI
      else if BX <> SI then Below := BX < SI
      else Below := AH < CH;
      if not Below then
      begin
        Borrow := 0;
        if AH < CH then Borrow := 1;
        AH := Byte(AH - CH);
        BX := SubB(BX, SI, Borrow);
        BP := SubB(BP, DI, Borrow);
      end;
      CF := Below;
    end
    else
    begin
      { 19FE4: SHL AH,1 / RCL BX,1 / RCL BP,1 }
      CF := (BP and $8000) <> 0;
      BP := (BP shl 1) or (BX shr 15);
      BX := (BX shl 1) or (AH shr 7);
      AH := Byte(AH shl 1);
      if not CF then
      begin
        Step := 0;
        Continue;
      end;
      Borrow := 0;
      if AH < CH then Borrow := 1;
      AH := Byte(AH - CH);
      BX := SubB(BX, SI, Borrow);
      BP := SubB(BP, DI, Borrow);
      CF := False;
    end;
    { 19FE0: RCL DX,1 }
    if (Q and $8000) <> 0 then
    begin
      Q := Word((Q shl 1) or Ord(CF));
      { 19FF5 }
      Dec(Count);
      if Count < 0 then Break;
      if Count = 1 then W1 := Q else W2 := Q;
      Q := 1;
      if Count = 0 then Q := $40;
      Step := 1;
    end
    else
    begin
      Q := Word((Q shl 1) or Ord(CF));
      Step := 1;
    end;
  end;
  { 1A003 }
  AX := Word(Q shl 6);
  BX := W2;
  DX := W1;
  CX := ExpWord;
  AX := not AX;
  BX := not BX;
  DX := DX xor $FFFF;
  if (DX and $8000) = 0 then
  begin
    DX := (DX shl 1) or (BX shr 15);
    BX := (BX shl 1) or (AX shr 15);
    AX := AX shl 1;
    Dec(CX);
  end;
  CX := Word(LongInt(CX) + $8080);
  RealDiv := FinishProduct(R, AX, BX, DX, CX);
end;

{ ---- 1A024: comparison ------------------------------------------------- }

function MagnitudeCompare(const A, B: TReal): Integer;
begin
  if Lo(A.W0) <> Lo(B.W0) then
  begin
    if Lo(A.W0) < Lo(B.W0) then MagnitudeCompare := -1 else MagnitudeCompare := 1;
  end
  else if Lo(A.W0) = 0 then
    MagnitudeCompare := 0
  else if A.W2 <> B.W2 then
  begin
    if A.W2 < B.W2 then MagnitudeCompare := -1 else MagnitudeCompare := 1;
  end
  else if A.W1 <> B.W1 then
  begin
    if A.W1 < B.W1 then MagnitudeCompare := -1 else MagnitudeCompare := 1;
  end
  else if Hi(A.W0) <> Hi(B.W0) then
  begin
    if Hi(A.W0) < Hi(B.W0) then MagnitudeCompare := -1 else MagnitudeCompare := 1;
  end
  else
    MagnitudeCompare := 0;
end;

function RCmp(A, B: TReal): Integer;
begin
  if ((A.W2 xor B.W2) and $8000) <> 0 then
  begin
    if (A.W2 and $8000) <> 0 then RCmp := -1 else RCmp := 1;
  end
  else if (A.W2 and $8000) = 0 then
    RCmp := MagnitudeCompare(A, B)
  else
    RCmp := -MagnitudeCompare(A, B);
end;

{ ---- 1A04E: LongInt to Real -------------------------------------------- }

function RFromLong(L: LongInt): TReal;
var
  AX, BX, DX, C: Word;
  Sign: Byte;
  R: TReal;
begin
  AX := Word(LongWord(L) and $FFFF);
  DX := Word(LongWord(L) shr 16);
  if (AX or DX) = 0 then
  begin
    MakeZero(R);
    RFromLong := R;
    Exit;
  end;
  Sign := Hi(DX);
  if (DX and $8000) <> 0 then
  begin
    AX := not AX;
    DX := not DX;
    C := 0;
    AX := AddC(AX, 1, C);
    DX := AddC(DX, 0, C);
  end;
  BX := AX;
  AX := $00A0;
  if DX = 0 then
  begin
    DX := BX;
    BX := 0;
    AX := $0090;
    if Hi(DX) = 0 then
    begin
      DX := DX shl 8;
      AX := $0088;
    end;
  end;
  while (DX and $8000) = 0 do
  begin
    Dec(AX);
    DX := (DX shl 1) or (BX shr 15);
    BX := BX shl 1;
  end;
  if (Sign and $80) = 0 then
    DX := DX and $7FFF;
  Store(R, AX, BX, DX);
  RFromLong := R;
end;

{ ---- 1A08D: Trunc (DoRound = False) and Round (DoRound = True) --------- }

function RealTruncRound(A: TReal; DoRound: Boolean; var Value: LongInt): Boolean;
var
  AX, DX, C, Last: Word;
  Count, Exponent, Sign: Byte;
begin
  RealTruncRound := True;
  AX := A.W1;
  DX := A.W2;
  Exponent := Lo(A.W0);
  if Exponent > $9F then
  begin
    RealTruncRound := False;
    Exit;
  end;
  Count := $9F - Exponent;
  if Count > $1F then
  begin
    Value := 0;
    Exit;
  end;
  Inc(Count);
  Sign := Hi(DX);
  DX := DX or $8000;
  if Count >= $11 then
  begin
    AX := DX;
    DX := 0;
    Dec(Count, $10);
  end;
  if Count >= 9 then
  begin
    AX := (AX shr 8) or ((DX and $FF) shl 8);
    DX := DX shr 8;
    Dec(Count, 8);
  end;
  repeat
    Last := AX and 1;
    AX := (AX shr 1) or ((DX and 1) shl 15);
    DX := DX shr 1;
    Dec(Count);
  until Count = 0;
  if (Last <> 0) and DoRound then
  begin
    C := 0;
    AX := AddC(AX, 1, C);
    DX := DX + C;
    if (DX and $8000) <> 0 then
    begin
      RealTruncRound := False;
      Exit;
    end;
  end;
  if (Sign and $80) <> 0 then
  begin
    AX := not AX;
    DX := not DX;
    C := 0;
    AX := AddC(AX, 1, C);
    DX := AddC(DX, 0, C);
  end;
  Value := LongInt((LongWord(DX) shl 16) or AX);
end;

{ ---- 1A15D: Int, 1A1AE: Frac ------------------------------------------ }

function RInt(A: TReal): TReal;
var
  AH, BX, DX: Word;
  Count: Byte;
  R: TReal;
begin
  if Lo(A.W0) >= $A8 then
  begin
    RInt := A;
    Exit;
  end;
  if Lo(A.W0) <= $80 then
  begin
    MakeZero(R);
    RInt := R;
    Exit;
  end;
  Count := Lo(A.W0) - $80;
  AH := 0;
  BX := 0;
  DX := 0;
  while Count >= 16 do
  begin
    AH := Hi(BX);
    BX := DX;
    DX := $FFFF;
    Dec(Count, 16);
  end;
  if Count >= 8 then
  begin
    AH := Lo(BX);
    BX := (BX shr 8) or ((DX and $FF) shl 8);
    DX := (DX shr 8) or $FF00;
    Dec(Count, 8);
  end;
  while Count > 0 do
  begin
    AH := (AH shr 1) or ((BX and 1) shl 7);
    BX := (BX shr 1) or ((DX and 1) shl 15);
    DX := (DX shr 1) or $8000;
    Dec(Count);
  end;
  DX := DX and A.W2;
  BX := BX and A.W1;
  AH := AH and Hi(A.W0);
  Store(R, (AH shl 8) or Lo(A.W0), BX, DX);
  RInt := R;
end;

function Negated(A: TReal): TReal;
begin
  A.W2 := A.W2 xor $8000;
  Negated := A;
end;

function AddNoCheck(A, B: TReal): TReal;
var
  R: TReal;
begin
  RealAdd(R, A, B);
  AddNoCheck := R;
end;

function MulNoCheck(A, B: TReal): TReal;
var
  R: TReal;
begin
  RealMul(R, A, B);
  MulNoCheck := R;
end;

function DivNoCheck(A, B: TReal): TReal;
var
  R: TReal;
begin
  RealDiv(R, A, B);
  DivNoCheck := R;
end;

function RFrac(A: TReal): TReal;
begin
  RFrac := AddNoCheck(A, Negated(RInt(A)));
end;

{ ---- Checked public arithmetic (runtime errors as in the original) ----- }

function RAdd(A, B: TReal): TReal;
var
  R: TReal;
begin
  if not RealAdd(R, A, B) then RunError(205);
  RAdd := R;
end;

function RSub(A, B: TReal): TReal;
begin
  RSub := AddNoCheck(A, Negated(B));
end;

function RMul(A, B: TReal): TReal;
var
  R: TReal;
begin
  if not RealMul(R, A, B) then RunError(205);
  RMul := R;
end;

function RDiv(A, B: TReal): TReal;
var
  R: TReal;
begin
  if Lo(B.W0) = 0 then RunError(200);
  if not RealDiv(R, A, B) then RunError(205);
  RDiv := R;
end;

function RTrunc(A: TReal): LongInt;
var
  V: LongInt;
begin
  if not RealTruncRound(A, False, V) then RunError(207);
  RTrunc := V;
end;

function RRound(A: TReal): LongInt;
var
  V: LongInt;
begin
  if not RealTruncRound(A, True, V) then RunError(207);
  RRound := V;
end;

{ ---- 1A1C2: square root by Newton iteration ---------------------------- }

function RSqrt(A: TReal): TReal;
var
  Guess, T, D: TReal;
  E, Threshold: Byte;
begin
  if Lo(A.W0) = 0 then
  begin
    RSqrt := A;
    Exit;
  end;
  if (A.W2 and $8000) <> 0 then RunError(207);
  Guess := A;
  E := Byte(Lo(A.W0) + $80);
  if (E and $80) <> 0 then E := (E shr 1) or $80 else E := E shr 1;
  E := Byte(E + $80);
  SetLow(Guess.W0, E);
  Threshold := Byte(E - $14);
  repeat
    T := DivNoCheck(A, Guess);
    T := AddNoCheck(T, Guess);
    SetLow(T.W0, Byte(Lo(T.W0) - 1));
    D := AddNoCheck(T, Negated(Guess));
    Guess := T;
  until Lo(D.W0) < Threshold;
  RSqrt := Guess;
end;

{ ---- 1A234: Sin, 1A221: Cos, with the 1A540/1A559 polynomial ---------- }

const
  TwoPi: TReal = (W0: $2183; W1: $DAA2; W2: $490F);
  MinusHalfPi: TReal = (W0: $2181; W1: $DAA2; W2: $C90F);
  RealOne: TReal = (W0: $0081; W1: 0; W2: 0);
  SinCoefficients: array[0..6] of TReal = (
    (W0: $9D58; W1: $9F39; W2: $D73F),
    (W0: $4360; W1: $309D; W2: $3092),
    (W0: $AA67; W1: $283F; W2: $D732),
    (W0: $B66E; W1: $1D2A; W2: $38EF),
    (W0: $0D74; W1: $00D0; W2: $D00D),
    (W0: $887A; W1: $8888; W2: $0888),
    (W0: $AB7E; W1: $AAAA; W2: $AAAA));

function SinPolynomial(X: TReal): TReal;
var
  Y, Acc: TReal;
  I: Integer;
begin
  Y := MulNoCheck(X, X);
  Acc := MulNoCheck(SinCoefficients[0], Y);
  for I := 1 to 6 do
    Acc := MulNoCheck(AddNoCheck(Acc, SinCoefficients[I]), Y);
  Acc := AddNoCheck(Acc, RealOne);
  SinPolynomial := MulNoCheck(Acc, X);
end;

function RSin(A: TReal): TReal;
var
  Bound, Magnitude: TReal;
  Negate: Boolean;
begin
  if Lo(A.W0) < $6C then
  begin
    RSin := A;
    Exit;
  end;
  Magnitude := A;
  Magnitude.W2 := Magnitude.W2 and $7FFF;
  if RCmp(Magnitude, TwoPi) >= 0 then
    A := MulNoCheck(RFrac(DivNoCheck(A, TwoPi)), TwoPi);
  if (A.W2 and $8000) <> 0 then
    A := AddNoCheck(A, TwoPi);
  Bound := TwoPi;
  SetLow(Bound.W0, Byte(Lo(Bound.W0) - 1));
  Negate := RCmp(A, Bound) >= 0;
  if Negate then
    A := AddNoCheck(A, Negated(Bound));
  SetLow(Bound.W0, Byte(Lo(Bound.W0) - 1));
  if RCmp(A, Bound) >= 0 then
  begin
    SetLow(Bound.W0, Byte(Lo(Bound.W0) + 1));
    A.W2 := A.W2 or $8000;
    A := AddNoCheck(A, Bound);
  end;
  if Lo(A.W0) >= $6C then
    A := SinPolynomial(A);
  if Negate and (Lo(A.W0) <> 0) then
    A.W2 := A.W2 xor $8000;
  RSin := A;
end;

function RCos(A: TReal): TReal;
begin
  A := AddNoCheck(A, MinusHalfPi);
  if Lo(A.W0) <> 0 then
    A.W2 := A.W2 xor $8000;
  RCos := RSin(A);
end;

{ ---- 1A5F7/1A5A8: Random, 1A62F: Randomize ----------------------------- }

function TPRandom(Range: Word): Word;
begin
  TPRandSeed := LongInt(LongWord(TPRandSeed) * LongWord($08088405) + 1);
  if Range = 0 then
    TPRandom := 0
  else
    TPRandom := Word(LongWord(TPRandSeed) shr 16) mod Range;
end;

{$IFDEF MSDOS}
procedure TPRandomize;
var
  TimeCX, TimeDX: Word;
begin
  asm
    mov ah, $2C
    int $21
    mov TimeCX, cx
    mov TimeDX, dx
  end;
  { The low word is CX (hour:minute), the high word DX (second:hundredths). }
  TPRandSeed := LongInt((LongWord(TimeDX) shl 16) or TimeCX);
end;

procedure TPSound(Hz: Word);
var
  Divisor: Word;
  Gate: Byte;
begin
  if Hz <= $12 then
    Exit;
  Divisor := Word(LongInt($1234DD) div Hz);
  Gate := Port[$61];
  { The control word is written only when the speaker was off. }
  if (Gate and 3) = 0 then
  begin
    Port[$61] := Gate or 3;
    Port[$43] := $B6;
  end;
  Port[$42] := Lo(Divisor);
  Port[$42] := Hi(Divisor);
end;

procedure TPNoSound;
begin
  Port[$61] := Port[$61] and $FC;
end;

function TPKeyPressed: Boolean;
var
  Waiting: Boolean;
begin
  Waiting := True;
  if TPScanCode = 0 then
    asm
      mov ah, 1
      int $16
      jnz @key
      mov Waiting, 0
    @key:
    end ['AX'];
  TPKeyPressed := Waiting;
end;

function TPReadKey: Char;
var
  Key: Word;
begin
  if TPScanCode <> 0 then
  begin
    TPReadKey := Chr(TPScanCode);
    TPScanCode := 0;
    Exit;
  end;
  asm
    xor ah, ah
    int $16
    mov Key, ax
  end ['AX'];
  if Lo(Key) <> 0 then
    TPReadKey := Chr(Lo(Key))
  else
  begin
    TPScanCode := Hi(Key);
    if TPScanCode <> 0 then
      TPReadKey := #0
    else
      { Ctrl-Break (AX = 0) reads as #3; the original then runs Crt's break
        check, which does nothing once the game has cleared CheckBreak. }
      TPReadKey := #3;
  end;
end;
{$ENDIF}

begin
  TPRandSeed := 0;
{$IFDEF MSDOS}
  TPScanCode := 0;
{$ENDIF}
end.
