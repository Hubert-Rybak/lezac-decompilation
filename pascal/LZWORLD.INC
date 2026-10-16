{ LZWORLD.INC -- main program, original linear 0x1346B..0x14529: aiming,
  camera, map probing (debris pieces and falling groups), impulse sharing,
  bonus rain, explosions and the moving platforms. }

{ 1346B }
{ Velocity (VelX, VelY) of length Speed in the direction (DistX,
  DistY); a zero direction gives a zero velocity. }
procedure AimVelocity(var DistX, DistY: Integer; var Speed: Word;
  var VelX, VelY: Integer);
var
  Factor, RX, RY: TReal;
begin
  RX := RFromLong(DistX);
  RY := RFromLong(DistY);
  if (RCmp(RX, RealZero) = 0) and (RCmp(RY, RealZero) = 0) then
    Factor := RealZero
  else
    Factor := RDiv(RFromLong(Speed),
      RSqrt(RAdd(RMul(RX, RX), RMul(RY, RY))));
  VelX := RTrunc(RMul(RX, Factor));
  VelY := RTrunc(RMul(RY, Factor));
end;

{ 13587 }
{ Centre the view on the map pixel (X, Y), clamp it to the map and
  apply the horizontal shake offset. CamX/CamY are signed here. }
procedure SetCamera(X, Y: Integer);
begin
  CamX := (X - ViewCenterX) and $FFF8;
  CamY := (Y - 80) and $FFF8;
  MapViewOfs := (Y div 8 - 10) * MapWidth + (X div 8 - ViewCells);
  FineX := X mod 8;
  FineY := Y mod 8;
  if Integer(CamX) < 0 then
  begin
    MapViewOfs := MapViewOfs - Integer(CamX) div 8;
    FineX := 0;
    CamX := 0;
  end
  else if Integer(CamX) > CamMaxX then
  begin
    MapViewOfs := MapViewOfs + Integer(CamMaxX - Integer(CamX)) div 8;
    FineX := 7;
    CamX := CamMaxX;
  end;
  if Integer(CamY) < 0 then
  begin
    MapViewOfs := MapViewOfs - Integer(CamY) div 8 * MapWidth;
    FineY := 0;
    CamY := 0;
  end
  else if Integer(CamY) > CamMaxY then
  begin
    MapViewOfs := MapViewOfs - Integer(Integer(CamY) - CamMaxY) div 8 * MapWidth;
    FineY := 7;
    CamY := CamMaxY;
  end;
  MapViewOfs := MapViewOfs;
  FineX := FineX + ShakeOffset;
end;

{ 1370E }
{ Set the map cell Cell in motion. A tag above $3FFF is a single
  debris piece (Hit = 1); otherwise all cells sharing the tag are found by
  growing a box until no tagged cell touches its border, and they become a
  falling group (Hit = 0). Tags with bit 15 set are already moving: nothing
  is changed, not even Hit or MotionOk. MotionOk = 0 when the table is full. }
procedure SetCellMoving(Cell: Word; VelX, VelY: Shortint; var Hit: Byte);
type
  TCells = array[0..32766] of Word;
  PCells = ^TCells;
var
  Count, RowBytes, TopLeft, BottomRight, TopRight, BottomLeft: Word;
  Cols, Rows, Tag: Word;
  Done, Up, Down, Left, Right: Boolean;
  Spare: array[1..2] of Byte;            { never referenced (frame size) }
begin
  Tag := PCells(MapB)^[Cell];
  RowBytes := MapWidth * 2;
  if Tag and $8000 = 0 then
    if Tag > $3FFF then
    begin
      Hit := 1;
      if DebrisLast < 1600 then
      begin
        Inc(DebrisLast);
        PCells(MapB)^[Cell] := PCells(MapB)^[Cell] or $8000;
        with Debris[DebrisLast] do
        begin
          CellIndex := Cell;
          VelocityX := VelX;
          VelocityY := VelY;
          TileCode := PByteWindow(MapA)^[Cell];
          PhaseXBits := 0;
          PhaseYBits := 0;
          AgeOrLife := 0;
          MapTag := Tag or $8000;
          AuxTimerOrFlags := 0;
        end;
        MotionOk := 1;
      end
      else
        MotionOk := 0;
    end
    else
    begin
      Cell := Cell * 2;
      TopLeft := Cell - RowBytes - 2;
      BottomRight := Cell;
      TopRight := TopLeft + 2;
      BottomLeft := BottomRight - 2;
      Hit := 0;
      Cols := 2;
      Rows := 2;
      repeat
        Done := True;
        Up := False;
        Down := False;
        Left := False;
        Right := False;
        asm
          les di, MapB
          mov di, TopLeft
          mov cx, Cols
        @TopScan:
          mov ax, es:[di]
          cmp ax, Tag
          je @TopFound
          add di, 2
          loop @TopScan
          jmp @LeftStart
        @TopFound:
          mov Done, 0
          mov Up, 1
          les di, MapB
        @LeftStart:
          mov di, TopLeft
          mov cx, Rows
        @LeftScan:
          mov ax, es:[di]
          cmp ax, Tag
          je @LeftFound
          add di, RowBytes
          loop @LeftScan
          jmp @RightStart
        @LeftFound:
          mov Done, 0
          mov Left, 1
          les di, MapB
        @RightStart:
          mov di, TopRight
          mov cx, Rows
        @RightScan:
          mov ax, es:[di]
          cmp ax, Tag
          je @RightFound
          add di, RowBytes
          loop @RightScan
          jmp @BottomStart
        @RightFound:
          mov Done, 0
          mov Right, 1
          les di, MapB
        @BottomStart:
          mov di, BottomLeft
          mov cx, Cols
        @BottomScan:
          mov ax, es:[di]
          cmp ax, Tag
          je @BottomFound
          add di, 2
          loop @BottomScan
          jmp @Scanned
        @BottomFound:
          mov Done, 0
          mov Down, 1
        @Scanned:
        end;
        if Up then
        begin
          TopLeft := TopLeft - RowBytes;
          TopRight := TopRight - RowBytes;
          Inc(Rows);
        end;
        if Down then
        begin
          BottomLeft := BottomLeft + RowBytes;
          BottomRight := BottomRight + RowBytes;
          Inc(Rows);
        end;
        if Left then
        begin
          TopLeft := TopLeft - 2;
          BottomLeft := BottomLeft - 2;
          Inc(Cols);
        end;
        if Right then
        begin
          TopRight := TopRight + 2;
          BottomRight := BottomRight + 2;
          Inc(Cols);
        end;
      until Done;
      TopLeft := TopLeft + RowBytes + 2;
      TopRight := TopRight + RowBytes - 2;
      BottomLeft := BottomLeft - RowBytes + 2;
      BottomRight := BottomRight - RowBytes - 2;
      if GroupLast < 250 then
      begin
        Inc(GroupLast);
        with Groups[GroupLast] do
        begin
          FirstWordMapOffset := TopLeft;
          LastWordMapOffset := BottomRight;
          MapTag := Tag or $8000;
          PhaseXBits := 0;
          PhaseYBits := 0;
          VelocityX := VelX;
          VelocityY := VelY;
          PreviousSpeedSum := Abs(Integer(VelX)) + Abs(Integer(VelY));
          SettleCounter := 0;
          Flags := 0;
        end;
        Count := 0;
        { mark the group's cells as moving and count them }
        asm
          les di, MapB
        @MarkRow:
          mov di, TopLeft
        @MarkCell:
          mov ax, es:[di]
          cmp ax, Tag
          jne @MarkNext
          xor ax, $8000
          inc Count
          mov es:[di], ax
        @MarkNext:
          add di, 2
          cmp di, TopRight
          jle @MarkCell
          mov ax, TopLeft
          add ax, RowBytes
          mov TopLeft, ax
          mov ax, TopRight
          add ax, RowBytes
          mov TopRight, ax
          cmp ax, BottomRight
          jle @MarkRow
        end;
        Groups[GroupLast].Weight := Count * 2;
        MotionOk := 1;
      end
      else
        MotionOk := 0;
    end;
end;

{ 13A56 }
{ TmpB := index of the last projectile slot 1..ProjLast whose cell is
  TmpA (unchanged if there is none). DX is not cleared before DIV: it holds
  the high word of (ProjLast - 1) * 11. }
procedure FindProjectileAt; assembler;
asm
  mov ax, ProjLast
  dec ax
  mov bx, 11
  mul bx
  mov bx, ax
@Scan:
  mov ax, word ptr [bx + Debris]
  cmp ax, TmpA
  jne @Previous
  mov ax, bx
  mov bx, 11
  div bx
  inc ax
  mov TmpB, ax
  jmp @Done
@Previous:
  sub bx, 11
  jae @Scan
@Done:
end;

{ 13A7E }
{ For the moving map tag TmpB find its group (tag below $4000) or
  debris piece 200..DebrisLast: TmpB := its index, ByteTemp2 := its
  VelocityX. A tag without bit 15 gives ByteTemp2 := 0. }
procedure FindMoverVX; assembler;
asm
  mov dx, TmpB
  test dx, $8000
  je @Still
  mov ax, dx
  and ax, $7FFF
  cmp ax, $4000
  jae @Piece
  mov ax, GroupLast
  dec ax
  cmp ax, $FFFF
  je @Done
  push dx
  mov bx, 15
  mul bx
  add ax, 4
  mov bx, ax
  mov cx, GroupLast
  pop dx
@GroupScan:
  mov ax, word ptr [bx + Groups]
  cmp ax, dx
  jne @GroupNext
  mov al, byte ptr [bx + Groups + 2]
  mov ByteTemp2, al
  xor dx, dx
  mov ax, bx
  mov bx, 15
  div bx
  inc ax
  mov TmpB, ax
  jmp @Done
@GroupNext:
  sub bx, 15
  loop @GroupScan
  jmp @Done
@Still:
  xor al, al
  mov ByteTemp2, al
  jmp @Done
@Piece:
  mov ax, DebrisLast
  dec ax
  cmp ax, 198
  je @Done
  push dx
  mov bx, 11
  mul bx
  add ax, 2
  mov bx, ax
  mov cx, DebrisLast
  sub cx, 199
  pop dx
@PieceScan:
  mov ax, word ptr [bx + Debris]
  cmp ax, dx
  jne @PieceNext
  mov al, byte ptr [bx + Debris + 2]
  mov ByteTemp2, al
  xor dx, dx
  mov ax, bx
  mov bx, 11
  div bx
  inc ax
  mov TmpB, ax
  jmp @Done
@PieceNext:
  sub bx, 11
  loop @PieceScan
@Done:
end;

{ 13B18 }
{ As 13A7E, but ByteTemp2 := VelocityY. }
procedure FindMoverVY; assembler;
asm
  mov dx, TmpB
  test dx, $8000
  je @Still
  mov ax, dx
  and ax, $7FFF
  cmp ax, $4000
  jae @Piece
  mov ax, GroupLast
  dec ax
  cmp ax, $FFFF
  je @Done
  push dx
  mov bx, 15
  mul bx
  add ax, 4
  mov bx, ax
  mov cx, GroupLast
  pop dx
@GroupScan:
  mov ax, word ptr [bx + Groups]
  cmp ax, dx
  jne @GroupNext
  mov al, byte ptr [bx + Groups + 3]
  mov ByteTemp2, al
  xor dx, dx
  mov ax, bx
  mov bx, 15
  div bx
  inc ax
  mov TmpB, ax
  jmp @Done
@GroupNext:
  sub bx, 15
  loop @GroupScan
  jmp @Done
@Still:
  xor al, al
  mov ByteTemp2, al
  jmp @Done
@Piece:
  mov ax, DebrisLast
  dec ax
  cmp ax, 198
  je @Done
  push dx
  mov bx, 11
  mul bx
  add ax, 2
  mov bx, ax
  mov cx, DebrisLast
  sub cx, 199
  pop dx
@PieceScan:
  mov ax, word ptr [bx + Debris]
  cmp ax, dx
  jne @PieceNext
  mov al, byte ptr [bx + Debris + 3]
  mov ByteTemp2, al
  xor dx, dx
  mov ax, bx
  mov bx, 11
  div bx
  inc ax
  mov TmpB, ax
  jmp @Done
@PieceNext:
  sub bx, 11
  loop @PieceScan
@Done:
end;

{ 13BB2 }
{ Momentum exchange along X: an object of weight Weight moving with
  VX hits the HitCount cells HitCells/HitOffsets. Still cells are set in
  motion (SetCellMoving), moving ones are looked up; every hit object and VX get
  the weighted mean velocity. HitRefs: group index, or 20000 + debris
  index. Exits early, VX unchanged, when SetCellMoving fails. }
procedure ShareImpulseX(Weight: Byte; var VX: Shortint);
var
  Sum: LongInt;
  CellWord, Total, I: Word;
  W, Hit: Byte;
  NewV: Shortint;
begin
  Total := Weight;
  Sum := Weight * VX;
  for I := 1 to HitCount do
  begin
    CellWord := HitCells[I];
    if CellWord and $8000 = 0 then
    begin
      SetCellMoving(HitOffsets[I] shr 1, 0, 0, Hit);
      if MotionOk = 0 then
        Exit;
      if Hit = 0 then
      begin
        HitRefs[I] := GroupLast;
        Total := Groups[GroupLast].Weight + Total;
      end
      else
      begin
        HitRefs[I] := DebrisLast + 20000;
        Inc(Total);
      end;
    end
    else
    begin
      TmpB := CellWord;
      FindMoverVX;
      if CellWord and $7FFF > $3FFF then
      begin
        TmpB := TmpB + 20000;
        W := 1;
      end
      else
        W := Groups[TmpB].Weight;
      HitRefs[I] := TmpB;
      Total := W + Total;
      Sum := Sum + W * ByteTemp2;
    end;
  end;
  NewV := Sum div Total;
  for I := 1 to HitCount do
  begin
    Total := HitRefs[I];
    if Total < 20000 then
      Groups[Total].VelocityX := NewV
    else
      Debris[Total - 20000].VelocityX := NewV;
  end;
  VX := NewV;
end;

{ 13D46 }
{ As 13BB2 along Y. }
procedure ShareImpulseY(Weight: Byte; var VY: Shortint);
var
  Sum: LongInt;
  CellWord, Total, I: Word;
  W, Hit: Byte;
  NewV: Shortint;
begin
  Total := Weight;
  Sum := Weight * VY;
  for I := 1 to HitCount do
  begin
    CellWord := HitCells[I];
    if CellWord and $8000 = 0 then
    begin
      SetCellMoving(HitOffsets[I] shr 1, 0, 0, Hit);
      if MotionOk = 0 then
        Exit;
      if Hit = 0 then
      begin
        HitRefs[I] := GroupLast;
        Total := Groups[GroupLast].Weight + Total;
      end
      else
      begin
        HitRefs[I] := DebrisLast + 20000;
        Inc(Total);
      end;
    end
    else
    begin
      TmpB := CellWord;
      FindMoverVY;
      if CellWord and $7FFF > $3FFF then
      begin
        TmpB := TmpB + 20000;
        W := 1;
      end
      else
        W := Groups[TmpB].Weight;
      HitRefs[I] := TmpB;
      Total := W + Total;
      Sum := Sum + W * ByteTemp2;
    end;
  end;
  NewV := Sum div Total;
  for I := 1 to HitCount do
  begin
    Total := HitRefs[I];
    if Total < 20000 then
      Groups[Total].VelocityY := NewV
    else
      Debris[Total - 20000].VelocityY := NewV;
  end;
  VY := NewV;
end;

{ 13EDA }
{ Advance the sub-cell phases MovePX/MovePY by MoveVX/MoveVY; a
  signed overflow moves CellStep one cell (one row for Y). }
procedure AdvanceMove; assembler;
asm
  mov al, MoveVX
  mov ah, MovePX
  mov bx, 1
  cmp al, 0
  jge @StepX
  mov bx, $FFFF
@StepX:
  add ah, al
  jno @StoreX
  sub ah, $80
  mov cx, CellStep
  add cx, bx
  mov CellStep, cx
@StoreX:
  mov MovePX, ah
  mov al, MoveVY
  mov ah, MovePY
  mov bx, MapWidth
  cmp al, 0
  jge @StepY
  neg bx
@StepY:
  add ah, al
  jno @StoreY
  sub ah, $80
  mov cx, CellStep
  add cx, bx
  mov CellStep, cx
@StoreY:
  mov MovePY, ah
end;

{ 13F27 }
{ Drop a random bonus tile ($67..$6F, never CountedTile) into an
  empty cell of the second map row and let it fall. }
procedure DropBonusTile;
type
  TCells = array[0..32766] of Word;
  PCells = ^TCells;
var
  Cell: Word;
begin
  Cell := TPRandom(MapWidth) + MapWidth;
  if PByteWindow(MapA)^[Cell] = 0 then
  begin
    ByteTemp := TPRandom(9) + $66 + 1;
    if ByteTemp = CountedTile then
      Inc(ByteTemp);
    PByteWindow(MapA)^[Cell] := ByteTemp;
    PCells(MapB)^[Cell] := NextTag;
    Inc(NextTag);
    SetCellMoving(Cell, 0, 0, ByteTemp);
  end;
end;

{ 1414A }
{ Explosion of bomb kind Kind (1..4) at map cell Cell: projectiles
  in a fan of directions, screen shake, flash and sound. }
procedure Explode(Cell: Word; Kind: Byte);
const
  { Real constants of the original, (W0: AX; W1: BX; W2: DX) }
  Deg15: TReal = (W0: $0084; W1: $0000; W2: $7000);
  Deg22: TReal = (W0: $0085; W1: $0000; W2: $3000);
  Deg30: TReal = (W0: $0085; W1: $0000; W2: $7000);
  Deg45: TReal = (W0: $0086; W1: $0000; W2: $3400);
  Deg90: TReal = (W0: $0087; W1: $0000; W2: $3400);
  Deg180: TReal = (W0: $0088; W1: $0000; W2: $3400);
  Deg270: TReal = (W0: $0089; W1: $0000; W2: $0700);
  Deg280: TReal = (W0: $0089; W1: $0000; W2: $0C00);
  Deg340: TReal = (W0: $0089; W1: $0000; W2: $2A00);
  Deg357: TReal = (W0: $0089; W1: $0000; W2: $3280);
  Deg360: TReal = (W0: $0089; W1: $0000; W2: $3400);
  TwoPi: TReal = (W0: $5C83; W1: $C28F; W2: $48F5);     { 6.28 }
var
  Origin: Word;
  Life, Flags: Byte;

  { 13FA6 }
  { One projectile every Step degrees from Angle until Angle > Last (at
    least one try); none are created once ProjLast has reached 198. }
  procedure Spray(At: Word; Angle, Last, Step: TReal; Mass: Shortint;
    Speed: Byte);
  begin
    Angle := RMul(RDiv(Angle, Deg360), TwoPi);
    Last := RMul(RDiv(Last, Deg360), TwoPi);
    Step := RMul(RDiv(Step, Deg360), TwoPi);
    repeat
      if ProjLast < 198 then
      begin
        Inc(ProjLast);
        ProjWeight[ProjLast] := Byte(Mass);
        with Debris[ProjLast] do
        begin
          CellIndex := At;
          VelocityX := RTrunc(RMul(RCos(Angle), RFromLong(Speed)));
          VelocityY := -RTrunc(RMul(RSin(Angle), RFromLong(Speed)));
          PhaseXBits := 0;
          PhaseYBits := 0;
          TileCode := $75;
          AgeOrLife := Life;
          AuxTimerOrFlags := Flags;
        end;
      end;
      Angle := RAdd(Angle, Step);
    until RCmp(Angle, Last) > 0;
  end;

begin
  Origin := Cell;
  if Kind > 1 then
  begin
    ShakeTimer := 2;
    Shaking := 1;
  end;
  case Kind of
    1:
      begin
        Life := 8;
        Flags := 5;
        Spray(Origin, RealZero, Deg340, Deg45, 1, $50);
        TmpB := $EA74;
        SoundRequest := 4;
      end;
    2:
      begin
        Life := 9;
        Flags := 5;
        Spray(Origin, RealZero, Deg357, Deg30, 1, $6E);
        TmpB := $EA7E;
        SoundRequest := 5;
      end;
    3:
      begin
        Life := 9;
        Flags := 3;
        Spray(Origin, RealZero, Deg357, Deg22, 9, $6E);
        TmpB := $EA88;
        SoundRequest := 6;
      end;
    4:
      begin
        Life := $3A;
        Flags := $0A;
        StartFlash(0, FlashRed, FlashBlack);
        Spray(Origin, Deg90, Deg180, Deg15, -35, $7E);
        Spray(Origin + 1, RealZero, Deg90, Deg15, -35, $7E);
        Spray(Origin + MapWidth, Deg180, Deg270, Deg15, -35, $7E);
        Spray(Origin + MapWidth + 1, Deg280, Deg360, Deg15, -35, $7E);
        SoundRequest := 7;
        TmpB := $EACE;
      end;
  end;
  RequestSound;
end;

{ 1432A }
{ Actor motion controllers (read by UpdateActor): ResultX/Y is the scaled
  offset of the sprite from a reference sprite (added to the actor's
  velocity) or, with ModeOrDrag = $FF, a point circling the sprite on the
  sine table (used as the actor's position). }
procedure UpdateMotions;
var
  I: Integer;
begin
  for I := 1 to MotionCount do
    with Motions[I], RenderEntries[PrimarySprite] do
      if ModeOrDrag <> $FF then
      begin
        ResultXBits := (Word(XBits - RenderEntries[ReferenceSprite].XBits) +
          BaseX) * DivisorOrPhaseStep;
        ResultYBits := (Word(YBits - RenderEntries[ReferenceSprite].YBits) +
          BaseY) * DivisorOrPhaseStep + BiasY;
      end
      else
      begin
        Phase := (Phase + DivisorOrPhaseStep) and $7F;
        ResultXBits := XBits + BaseX +
          RTrunc(RMul(RFromLong(AmplitudeX), SinTable[(Phase + $20) and $7F]));
        ResultYBits := YBits + BaseY +
          RTrunc(RMul(RFromLong(AmplitudeY), SinTable[Phase]));
      end;
end;
