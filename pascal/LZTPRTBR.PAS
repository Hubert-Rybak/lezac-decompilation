library LZTPRTBR;
{ MODERN HOST VERIFICATION SCAFFOLDING ONLY: C-ABI exports of LZTPRT for the
  ctypes differential test against the original runtime instructions. }
{$MODE TP}
{$R-}{$Q-}

uses LZTPRT;

type
  PReal = ^TReal;
  PLong = ^LongInt;

function lz_real_add(A, B, R: PReal): Integer; cdecl;
begin
  lz_real_add := Ord(RealAdd(R^, A^, B^));
end;

function lz_real_mul(A, B, R: PReal): Integer; cdecl;
begin
  lz_real_mul := Ord(RealMul(R^, A^, B^));
end;

function lz_real_div(A, B, R: PReal): Integer; cdecl;
begin
  lz_real_div := Ord(RealDiv(R^, A^, B^));
end;

function lz_real_cmp(A, B: PReal): Integer; cdecl;
begin
  lz_real_cmp := RCmp(A^, B^);
end;

procedure lz_real_from_long(L: LongInt; R: PReal); cdecl;
begin
  R^ := RFromLong(L);
end;

function lz_real_trunc_round(A: PReal; DoRound: Integer; V: PLong): Integer; cdecl;
begin
  lz_real_trunc_round := Ord(RealTruncRound(A^, DoRound <> 0, V^));
end;

procedure lz_real_int(A, R: PReal); cdecl;
begin
  R^ := RInt(A^);
end;

procedure lz_real_frac(A, R: PReal); cdecl;
begin
  R^ := RFrac(A^);
end;

procedure lz_real_sqrt(A, R: PReal); cdecl;
begin
  R^ := RSqrt(A^);
end;

procedure lz_real_sin(A, R: PReal); cdecl;
begin
  R^ := RSin(A^);
end;

procedure lz_real_cos(A, R: PReal); cdecl;
begin
  R^ := RCos(A^);
end;

function lz_random(Range: Word; Seed: PLong): Word; cdecl;
begin
  TPRandSeed := Seed^;
  lz_random := TPRandom(Range);
  Seed^ := TPRandSeed;
end;

exports
  lz_real_add, lz_real_mul, lz_real_div, lz_real_cmp, lz_real_from_long,
  lz_real_trunc_round, lz_real_int, lz_real_frac, lz_real_sqrt, lz_real_sin,
  lz_real_cos, lz_random;

begin
end.
