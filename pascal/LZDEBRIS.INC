{ LZDEBRIS.INC -- main program, original linear 0x1452A..0x1573F: flying
  projectiles and debris pieces, the scanning helpers of the falling
  building groups, the groups themselves, the energy bar and two small
  helpers of the actor code. }

{ 145FA  Flying objects. Slots 1..ProjLast of Debris are projectiles (tile
  $75, owner weight in ProjWeight): they share their momentum with what they
  hit and set it in motion. Slots 200..DebrisLast are loose debris pieces:
  they fall, slide, push what they land on and settle back into the map. }
procedure UpdateDebris;
type
  TCells = array[0..32766] of Word;
  PCells = ^TCells;
label
  Settle;
var
  I: Word;                 { bp-02 }
  Tag: Word;               { bp-04 }
  Above: Word;             { bp-06 }
  Cell: Word;              { bp-08 }
  NewCell: Word;           { bp-0A }
  PieceTag: Word;          { bp-0C }
  Spare: Word;             { bp-0E never referenced (frame size) }
  VX: Shortint;            { bp-0F }
  VY: Shortint;            { bp-10 }
  T: Byte;                 { bp-11 }
  Probe: Byte;             { bp-12 }
  Owner: Byte;             { bp-13 }
  Tile: Byte;              { bp-14 }
  Blocked: Boolean;        { bp-15 }

  { 1452A  Remove projectile Index: Debris[Index + 1..ProjLast] and
    ProjWeight move down one slot. }
  procedure RemoveProjectile(Index: Integer);
  begin
    if LongInt(Index) < LongInt(ProjLast) then
      asm
        push ds
        pop es
        mov ax, ProjLast
        sub ax, Index
        mov bx, 11
        mul bx
        push ax
        mov di, DebrisBase
        mov ax, Index
        dec ax
        mul bx
        add di, ax
        mov si, di
        add si, 11
        pop cx
        rep movsb
        mov bx, ProjLast
        sub bx, Index
        mov cx, bx
        mov bx, Index
        dec bx
      @Shift:
        mov al, byte ptr [bx + ProjWeight + 1]
        mov byte ptr [bx + ProjWeight], al
        inc bx
        loop @Shift
      end;
    Dec(ProjLast);
  end;

  { 1458D  Remove debris piece Index: Debris[Index + 1..DebrisLast] move
    down one slot and the caller's index I goes back with them. }
  procedure RemoveDebris(Index: Integer);
  begin
    if LongInt(Index) < LongInt(DebrisLast) then
      asm
        push ds
        pop es
        mov ax, DebrisLast
        sub ax, Index
        mov bx, 11
        mul bx
        push ax
        mov di, DebrisBase
        mov ax, Index
        dec ax
        mul bx
        add di, ax
        mov si, di
        add si, 11
        pop cx
        rep movsb
      end;
    if LongInt(I) >= LongInt(Index) then
      Dec(I);
    Dec(DebrisLast);
  end;

begin
  { projectiles, last to first }
  I := ProjLast;
  while I <> 0 do
  begin
    with Debris[I] do
    begin
      Cell := CellIndex;
      MoveVX := VelocityX;
      MoveVY := VelocityY;
      MovePX := PhaseXBits;
      MovePY := PhaseYBits;
    end;
    CellStep := 0;
    Blocked := False;
    AdvanceMove;
    if CellStep <> 0 then
    begin
      NewCell := Cell + CellStep;
      T := Mem[MapSeg:NewCell];
      if T = $66 then
      begin
        if PCells(MapB)^[NewCell] > $7FFF then
          PByteWindow(MapA)^[NewCell] := $FF
        else
        begin
          PByteWindow(MapA)^[NewCell] := 0;
          PCells(MapB)^[NewCell] := 0;
        end;
        Explode(NewCell, 1);
      end;
      if (T = 0) or (T = $75) then
      begin
        if Mem[MapSeg:Cell] = $75 then
          Mem[MapSeg:Cell] := 0;
        Mem[MapSeg:NewCell] := Debris[I].TileCode;
        Debris[I].CellIndex := NewCell;
      end
      else
      begin
        Tag := PCells(MapB)^[NewCell];
        if Tag > 0 then
        begin
          if (Tag and $8000) = 0 then
          begin
            { a still cell: set it in motion; it is the newest group or
              debris piece }
            SetCellMoving(NewCell, 0, 0, Probe);
            VX := 0;
            VY := 0;
            if MotionOk = 0 then
              goto Settle;
            if Probe = 0 then
            begin
              TmpB := GroupLast;
              T := Groups[TmpB].Weight;
            end
            else
            begin
              T := 1;
              TmpB := DebrisLast;
            end;
          end
          else
          begin
            { already moving: find it and its velocity }
            TmpB := Tag;
            FindMoverVX;
            VX := ByteTemp2;
            TmpB := Tag;
            FindMoverVY;
            VY := ByteTemp2;
            if (Tag and $7FFF) < $4000 then
              T := Groups[TmpB].Weight
            else
            begin
              T := 1;
              if Debris[I].AuxTimerOrFlags > 0 then
                PByteWindow(MapA)^[NewCell] := $FF;
            end;
          end;
          { weighted mean of the two velocities (16-bit, signed division) }
          Owner := ProjWeight[I];
          VX := Integer(Integer(Owner * MoveVX) + Integer(T * VX)) div
            Integer(Owner + T);
          VY := Integer(Integer(Owner * MoveVY) + Integer(T * VY)) div
            Integer(Owner + T);
          if (Tag and $7FFF) < $4000 then
            with Groups[TmpB] do
            begin
              VelocityX := VX;
              VelocityY := VY;
            end
          else
            with Debris[TmpB] do
            begin
              VelocityX := VX;
              VelocityY := VY;
            end;
        end;
      end;
    end
    else
      Blocked := True;
  Settle:
    with Debris[I] do
    begin
      VelocityY := MoveVY;
      PhaseXBits := MovePX;
      PhaseYBits := MovePY;
      VelocityX := MoveVX;
      if AuxTimerOrFlags > 0 then
        Dec(AuxTimerOrFlags);
      Dec(AgeOrLife);
      if AgeOrLife = 0 then
      begin
        if Mem[MapSeg:CellIndex] = $75 then
          Mem[MapSeg:CellIndex] := 0;
        RemoveProjectile(I);
      end;
    end;
    Dec(I);
  end;
  { debris pieces; DebrisLast shrinks while they settle }
  I := 199;
  if DebrisLast >= 200 then
  begin
    I := 200;
    while I <= DebrisLast do
    begin
      with Debris[I] do
      begin
        Cell := CellIndex;
        MoveVX := VelocityX;
        MoveVY := VelocityY;
        MovePX := PhaseXBits;
        MovePY := PhaseYBits;
        Tile := TileCode;
      end;
      PieceTag := Debris[I].MapTag;
      if (PieceTag > $FFBC) and (Tile <= $66) then
      begin
        Tile := $76;
        Debris[I].TileCode := $76;
        TmpB := $27;
        SoundRequest := 5;
        RequestSound;
      end;
      { tiles $76..$78 crumble; then a bonus tile or nothing ($FF) }
      if Tile >= $76 then
      begin
        Inc(Tile);
        Inc(Debris[I].TileCode);
        if Tile = $79 then
        begin
          if PieceTag > $FFBC then
            T := TPRandom(5) + $6B
          else
            T := $FF;
          Debris[I].TileCode := T;
          Tile := T;
        end;
        PByteWindow(MapA)^[Cell] := Tile;
      end;
      if Mem[MapSeg:Cell] = $FF then
      begin
        Mem[MapSeg:Cell] := 0;
        RemoveDebris(I);
        PCells(MapB)^[Cell] := 0;
        if PCells(MapB)^[Word(Cell - MapWidth)] > 0 then
          SetCellMoving(Word(Cell - MapWidth), 0, 0, Probe);
      end
      else
      begin
        CellStep := 0;
        Blocked := False;
        AdvanceMove;
        if CellStep = 0 then
          Blocked := True;
        if Mem[MapSeg:Word(Cell + MapWidth)] = 0 then
        begin
          { nothing below: fall }
          if MoveVY < $7B then
            Inc(MoveVY, 4);
          Debris[I].AgeOrLife := 0;
        end
        else
        begin
          if not Blocked then
            if Abs(Integer(MoveVX)) < 1 then
              MoveVX := 0
            else if MoveVX > 0 then
              Dec(MoveVX)
            else
              Inc(MoveVX);
          if (MoveVY > 0) and (MoveVY > $3C) and (Tile > $66) and
            (Word(FrameCount + I) mod 6 > 2) then
          begin
            PByteWindow(MapA)^[Cell] := $76;
            Debris[I].TileCode := $76;
            Tile := $76;
            SoundRequest := 2;
            TmpB := $21;
            RequestSound;
          end;
        end;
        if CellStep <> 0 then
        begin
          NewCell := Cell + CellStep;
          T := Mem[MapSeg:NewCell];
          if T = 0 then
          begin
            Debris[I].AgeOrLife := 0;
            Mem[MapSeg:NewCell] := Tile;
            Mem[MapSeg:Cell] := 0;
            PCells(MapB)^[NewCell] := PieceTag;
            PCells(MapB)^[Cell] := 0;
            Debris[I].CellIndex := NewCell;
            if LongInt(CellStep) <> -LongInt(MapWidth) then
            begin
              { moved away from below a still cell: let it fall too }
              Above := PCells(MapB)^[Word(Cell - MapWidth)];
              if (Above > 0) and (Above < $8000) then
              begin
                SetCellMoving(Word(Cell - MapWidth), 0, 0, T);
                Debris[I].AuxTimerOrFlags :=
                  Debris[I].AuxTimerOrFlags or $80;
              end;
            end;
          end
          else
          begin
            Blocked := True;
            if MoveVY > 0 then
            begin
              MoveVX := MoveVX + TPRandom(30) - 15;
              TmpB := TPRandom(8) + $EA61;
              SoundRequest := 1;
              RequestSound;
              MoveVY := 0;
            end;
            Tag := PCells(MapB)^[NewCell];
            if Tag > 0 then
            begin
              HitCount := 1;
              HitOffsets[1] := NewCell shl 1;
              HitCells[1] := Tag;
              ShareImpulseX(1, MoveVX);
              HitCells[1] := Tag or $8000;
              ShareImpulseY(1, MoveVY);
            end
            else
              MoveVX := 0;
          end;
        end
        else
          Blocked := True;
        with Debris[I] do
        begin
          VelocityY := MoveVY;
          PhaseXBits := MovePX;
          PhaseYBits := MovePY;
          VelocityX := MoveVX;
          if Blocked then
            Inc(AgeOrLife);
          if AgeOrLife = 100 then
          begin
            { stuck for 100 frames: becomes part of the map again }
            PCells(MapB)^[CellIndex] := PCells(MapB)^[CellIndex] and $7FFF;
            RemoveDebris(I);
          end;
        end;
      end;
      Inc(I);
    end;
  end;
end;

{ 14D3C  Scan the cells tagged GroupTag in the box GroupLeft (cell),
  GroupCols x GroupRows: for each one whose neighbour at CellStep is a
  non-empty cell of another tag, ByteTemp2 := 1 and TmpB/TmpA := smallest/
  largest map column of that neighbour (signed compares). Leaves CellStep2
  = CellStep and CellStep doubled (a byte step in MapB). }
procedure ScanGroupSupport; assembler;
asm
  mov TmpB, 10000
  mov TmpA, 0
  mov ByteTemp2, 0
  les di, MapB
  mov ax, CellStep
  mov CellStep2, ax
  shl ax, 1
  mov bx, ax
  mov CellStep, ax
  mov di, GroupLeft
  shl di, 1
  mov cx, GroupRows
@Row:
  push cx
  push di
  mov cx, GroupCols
@Cell:
  mov ax, word ptr es:[di]
  cmp ax, GroupTag
  jne @Next
  mov dx, word ptr es:[bx + di]
  mov es, MapSeg
  shr di, 1
  mov bx, CellStep2
  mov al, byte ptr es:[bx + di]
  shl di, 1
  mov bx, CellStep
  mov es, MapBSeg
  cmp al, 0
  je @Next
  cmp dx, GroupTag
  je @Next
  mov ByteTemp2, 1
  xor dx, dx
  mov ax, di
  shr ax, 1
  add ax, CellStep2
  push bx
  mov bx, MapWidth
  div bx
  pop bx
  cmp dx, TmpB
  jge @NotLeft
  mov TmpB, dx
@NotLeft:
  cmp dx, TmpA
  jle @Next
  mov TmpA, dx
@Next:
  inc di
  inc di
  loop @Cell
  pop di
  pop cx
  add di, RowBytes
  loop @Row
end;

{ 14DD3  As 14D3C, but only ByteTemp2 := 1 at the first such neighbour
  (and stop). TmpB/TmpA are only reset. }
procedure GroupBlocked; assembler;
asm
  mov TmpB, 10000
  mov TmpA, 0
  mov ByteTemp2, 0
  les di, MapB
  mov ax, CellStep
  mov CellStep2, ax
  shl ax, 1
  mov bx, ax
  mov CellStep, ax
  mov di, GroupLeft
  shl di, 1
  mov cx, GroupRows
@Row:
  push cx
  push di
  mov cx, GroupCols
@Cell:
  mov ax, word ptr es:[di]
  cmp ax, GroupTag
  jne @Next
  mov dx, word ptr es:[bx + di]
  mov es, MapSeg
  shr di, 1
  mov bx, CellStep2
  mov al, byte ptr es:[bx + di]
  shl di, 1
  mov bx, CellStep
  mov es, MapBSeg
  cmp al, 0
  je @Next
  cmp dx, GroupTag
  je @Next
  mov ByteTemp2, 1
  pop di
  pop cx
  jmp @Done
@Next:
  inc di
  inc di
  loop @Cell
  pop di
  pop cx
  add di, RowBytes
  loop @Row
@Done:
end;

{ 14E48  As 14D3C, collecting the neighbours instead: ByteTemp2 := 1 if any
  non-empty neighbour of another tag exists; each distinct non-zero
  neighbour tag is appended to HitCells (its MapB byte offset to
  HitOffsets), HitCount counting them. Exits with BX = doubled CellStep. }
procedure CollectGroupContacts; assembler;
asm
  mov ByteTemp2, 0
  les di, MapB
  mov ax, CellStep
  mov CellStep2, ax
  shl ax, 1
  mov bx, ax
  mov CellStep, ax
  mov di, GroupLeft
  shl di, 1
  mov cx, GroupRows
@Row:
  push cx
  push di
  mov cx, GroupCols
@Cell:
  mov ax, word ptr es:[di]
  cmp ax, GroupTag
  jne @Next
  mov dx, word ptr es:[bx + di]
  mov es, MapSeg
  shr di, 1
  mov bx, CellStep2
  mov al, byte ptr es:[bx + di]
  shl di, 1
  mov bx, CellStep
  mov es, MapBSeg
  cmp al, 0
  je @Next
  cmp dx, GroupTag
  je @Next
  mov ByteTemp2, 1
  cmp dx, 0
  je @Next
  mov ax, di
  add ax, bx
  push bx
  mov bx, HitCount
  shl bx, 1
  cmp bx, 0
  je @Append
  push bx
  push ax
@Search:
  dec bx
  dec bx
  mov ax, word ptr [bx + HitCells]
  cmp ax, dx
  jne @Other
  pop ax
  pop bx
  pop bx
  jmp @Next
@Other:
  cmp bx, 0
  jne @Search
  pop ax
  pop bx
@Append:
  mov word ptr [bx + HitCells], dx
  mov word ptr [bx + HitOffsets], ax
  pop bx
  inc HitCount
@Next:
  inc di
  inc di
  loop @Cell
  pop di
  pop cx
  add di, RowBytes
  loop @Row
end;

{ 14EE7  Move the group GroupTag one step CellStep (MapA and MapB) unless
  14E48 finds an obstacle (ByteTemp2 = 1); Flag := 1 when it moved. The
  scan order follows the direction so that no cell is overwritten; the box
  GroupLeft/GroupTop/GroupBottom moves along. Uses the BX left by 14E48. }
procedure StepGroup; assembler;
asm
  mov Flag, 0
  mov HitCount, 0
  call CollectGroupContacts
  mov al, ByteTemp2
  cmp al, 1
  jne @Free
  jmp @Done
@Free:
  les di, MapB
  mov Flag, 1
  mov ax, CellStep2
  neg ax
  cmp ax, MapWidth
  jne @NotUp
  mov ax, GroupLeft
  mov ScanStart, ax
  mov ScanStep, 2
  mov ax, RowBytes
  mov RowStep, ax
  jmp @Scan
@NotUp:
  neg ax
  cmp ax, MapWidth
  jne @NotDown
  mov ax, GroupBottom
  mov ScanStart, ax
  mov ScanStep, $FFFE
  mov ax, RowBytes
  neg ax
  mov RowStep, ax
  jmp @Scan
@NotDown:
  cmp ax, 1
  jne @Left
  mov ax, GroupTop
  mov ScanStart, ax
  mov ScanStep, $FFFE
  mov ax, RowBytes
  mov RowStep, ax
  jmp @Scan
@Left:
  mov ax, GroupLeft
  mov ScanStart, ax
  mov ScanStep, 2
  mov ax, RowBytes
  mov RowStep, ax
@Scan:
  mov di, ScanStart
  shl di, 1
  mov cx, GroupRows
@Row:
  push cx
  push di
  mov cx, GroupCols
@Cell:
  mov ax, word ptr es:[di]
  cmp ax, GroupTag
  jne @Next
  mov word ptr es:[bx + di], ax
  mov word ptr es:[di], 0
  shr di, 1
  mov bx, CellStep2
  mov es, MapSeg
  xor al, al
  xchg byte ptr es:[di], al
  mov byte ptr es:[bx + di], al
  mov bx, CellStep
  shl di, 1
  mov es, MapBSeg
@Next:
  add di, ScanStep
  loop @Cell
  pop di
  pop cx
  add di, RowStep
  loop @Row
  mov ax, GroupLeft
  add ax, CellStep2
  mov GroupLeft, ax
  mov ax, GroupTop
  add ax, CellStep2
  mov GroupTop, ax
  mov ax, GroupBottom
  add ax, CellStep2
  mov GroupBottom, ax
@Done:
end;

{ 14FDA  Break the group GroupTag apart (box from GroupLeft, here a MapB
  byte offset): every cell gets a new tag NextTag, rubble tile $47/$49
  (FrameCount bit 1) and becomes a debris piece with a random velocity
  around MoveVX/MoveVY; Destroyed counts the cells.
  The original body is assembler around one Pascal statement
    SetCellMoving(TmpB, MoveVX + Random(20) - 10, MoveVY - Random(40), ByteTemp)
  (32-bit sums, low byte pushed); that call is written out here as the
  instructions it compiled to, so the order of the two Random calls and the
  values kept on the stack across it are exact. PUSH imm (186+) is written
  MOV AX,imm / PUSH AX: AX is overwritten by Random right after. }
procedure ShatterGroup;
begin
  asm
    les di, MapB
    mov di, GroupLeft
    mov cx, GroupRows
  @Row:
    push cx
    push di
    mov cx, GroupCols
  @Cell:
    mov ax, word ptr es:[di]
    cmp ax, GroupTag
    jne @Next
    mov ax, NextTag
    mov word ptr es:[di], ax
    inc Destroyed
    push es
    mov es, MapSeg
    shr di, 1
    mov al, $47
    mov dx, FrameCount
    and dx, 2
    add al, dl
    mov byte ptr es:[di], al
    mov TmpB, di
    shl di, 1
    inc NextTag
    push di
    push cx
    push word ptr [TmpB]
    mov ax, 20
    push ax
    call TPRandom
    xor dx, dx
    mov cx, ax
    mov bx, dx
    mov al, MoveVX
    cbw
    cwd
    add ax, cx
    adc dx, bx
    sub ax, 10
    sbb dx, 0
    push ax
    mov ax, 40
    push ax
    call TPRandom
    xor dx, dx
    mov cx, ax
    mov bx, dx
    mov al, MoveVY
    cbw
    cwd
    sub ax, cx
    sbb dx, bx
    push ax
    mov di, offset ByteTemp
    push ds
    push di
    call SetCellMoving
    pop cx
    pop di
    pop es
  @Next:
    add di, 2
    dec cx
    cmp cx, 0
    je @RowDone
    jmp @Cell
  @RowDone:
    pop di
    pop cx
    add di, RowBytes
    dec cx
    cmp cx, 0
    je @Done
    jmp @Row
  @Done:
  end;
end;

{ 15102  Falling building groups, last to first: move each group by its
  velocity, share momentum with what it hits, let it slide off an edge it
  overhangs, and break it into debris when its speed changes abruptly.
  A group at rest for 95 frames becomes part of the map again. }
procedure UpdateGroups;
var
  G: Word;                 { bp-02 }
  K: Word;                 { bp-04 }
  Half: Word;              { bp-06 }
  LeftCol: Word;           { bp-08 }
  RightCol: Word;          { bp-0A }
  VX: Shortint;            { bp-0B }
  VY: Shortint;            { bp-0C }
  PX: Byte;                { bp-0D }
  PY: Byte;                { bp-0E }
  Fl: Byte;                { bp-0F }
  SavedStep: Integer;      { bp-12 }

  { 1508B  Remove group Index (Groups[Index + 1..GroupLast] move down one
    slot) and clear bit 15 of its cells: rows from GroupLeft to GroupTop
    (MapB byte offsets, signed compares) until GroupTop passes GroupBottom. }
  procedure RemoveGroup(Index: Word);
  begin
    if Index < GroupLast then
      asm
        push ds
        pop es
        mov ax, GroupLast
        sub ax, Index
        mov bx, 15
        mul bx
        push ax
        mov di, GroupBase
        mov ax, Index
        dec ax
        mul bx
        add di, ax
        mov si, di
        add si, 15
        pop cx
        rep movsb
      end;
    asm
      les di, MapB
    @Row:
      mov di, GroupLeft
    @Cell:
      mov ax, word ptr es:[di]
      cmp ax, GroupTag
      jne @Next
      and ax, $7FFF
      mov word ptr es:[di], ax
    @Next:
      add di, 2
      cmp di, GroupTop
      jle @Cell
      mov ax, GroupLeft
      add ax, RowBytes
      mov GroupLeft, ax
      mov ax, GroupTop
      add ax, RowBytes
      mov GroupTop, ax
      cmp ax, GroupBottom
      jle @Row
    end;
    Dec(GroupLast);
  end;

begin
  G := GroupLast;
  RowBytes := MapWidth shl 1;
  repeat
    with Groups[G] do
    begin
      GroupLeft := FirstWordMapOffset shr 1;
      VX := VelocityX;
      VY := VelocityY;
      PX := PhaseXBits;
      PY := PhaseYBits;
      GroupBottom := LastWordMapOffset shr 1;
      GroupTag := MapTag;
    end;
    MoveVX := VX;
    MoveVY := VY;
    { box: GroupLeft top-left cell, GroupTop top-right, GroupBottom
      bottom-right (unsigned arithmetic) }
    GroupTop := Word(GroupBottom) mod MapWidth + GroupLeft -
      GroupLeft mod MapWidth;
    GroupCols := Word(GroupTop) - GroupLeft + 1;
    GroupRows := Word(Word(GroupBottom) - Word(GroupTop)) div MapWidth + 1;
    { horizontal step }
    CellStep := 0;
    Flag := 0;
    asm
      mov al, VX
      mov ah, PX
      mov bx, 1
      cmp al, 0
      jge @StepX
      mov bx, $FFFF
    @StepX:
      add ah, al
      jno @StoreX
      sub ah, $80
      mov CellStep, bx
    @StoreX:
      mov PX, ah
    end;
    if CellStep <> 0 then
    begin
      if Groups[G].Flags < $80 then
      begin
        { set still cells resting on top in motion first }
        SavedStep := CellStep;
        CellStep := -LongInt(MapWidth);
        HitCount := 0;
        CollectGroupContacts;
        if ByteTemp2 = 1 then
          for K := 1 to HitCount do
            if HitCells[K] < $8000 then
              SetCellMoving(HitOffsets[K] shr 1, 0, 1, Fl);
        CellStep := SavedStep;
      end
      else
        Groups[G].Flags := Groups[G].Flags and $7F;
      StepGroup;
      if HitCount > 0 then
        ShareImpulseX(Groups[G].Weight, VX);
    end;
    { vertical step }
    CellStep := 0;
    asm
      mov al, VY
      mov ah, PY
      mov bx, MapWidth
      cmp al, 0
      jge @StepY
      neg bx
    @StepY:
      add ah, al
      jno @StoreY
      sub ah, $80
      mov CellStep, bx
    @StoreY:
      mov PY, ah
    end;
    if CellStep <> 0 then
    begin
      if (CellStep > 0) and (Groups[G].Flags < $80) then
      begin
        Groups[G].Flags := Groups[G].Flags or $80;
        SavedStep := CellStep;
        CellStep := -LongInt(MapWidth);
        HitCount := 0;
        CollectGroupContacts;
        if ByteTemp2 = 1 then
          for K := 1 to HitCount do
            if HitCells[K] < $8000 then
              SetCellMoving(HitOffsets[K] shr 1, 0, 1, Fl);
        CellStep := SavedStep;
      end
      else if CellStep <= 0 then
        Groups[G].Flags := Groups[G].Flags and $7F;
      StepGroup;
      if (ByteTemp2 = 1) and (VY > 0) then
      begin
        { landed }
        VY := 0;
        ShakeTimer := 3;
        Shaking := 1;
        TmpB := TPRandom(8) + $EA61;
        SoundRequest := 1;
        RequestSound;
      end;
      if HitCount > 0 then
        ShareImpulseY(Groups[G].Weight, VY);
    end;
    { support below: columns TmpB..TmpA against the middle half }
    LeftCol := GroupLeft mod MapWidth;
    RightCol := Word(GroupTop) mod MapWidth;
    Half := Word(RightCol - LeftCol) shr 1;
    LeftCol := LeftCol + Half;
    RightCol := RightCol - Half;
    CellStep := MapWidth;
    ScanGroupSupport;
    if ByteTemp2 = 0 then
    begin
      if VY < $7B then
        Inc(VY, 4);
      Flag := 1;
    end;
    Fl := Groups[G].Flags;
    if ByteTemp2 = 1 then
    begin
      if (Abs(Integer(VY)) < 10) and (Abs(Integer(VX)) < 30) then
      begin
        if (TmpB > LeftCol) and ((Fl and 2) = 0) then
        begin
          CellStep := -1;
          GroupBlocked;
          if ByteTemp2 = 0 then
          begin
            VX := -15;
            Groups[G].Flags := Fl or 1;
          end
          else
            Groups[G].Flags := Fl and $FC;
        end
        else if (TmpA < RightCol) and ((Fl and 1) = 0) then
        begin
          CellStep := 1;
          GroupBlocked;
          if ByteTemp2 = 0 then
          begin
            VX := 15;
            Groups[G].Flags := Fl or 2;
          end
          else
            Groups[G].Flags := Fl and $FC;
        end;
        if (TmpB <= LeftCol) and (TmpA >= RightCol) then
          Groups[G].Flags := Fl and $FC;
      end;
      if VX <> 0 then
        if VX > 0 then
          VX := VX - 1
        else
          VX := VX + 1;
    end
    else
      Groups[G].Flags := Fl and $FC;
    if Flag = 1 then
      Groups[G].SettleCounter := 0;
    Inc(Groups[G].SettleCounter);
    GroupLeft := GroupLeft shl 1;
    GroupBottom := GroupBottom shl 1;
    TmpB := Abs(Integer(VX)) + Abs(Integer(VY));
    TmpA := Abs(Integer(Groups[G].PreviousSpeedSum - TmpB));
    if TmpA > 63 then
    begin
      { impact: the group falls apart; GroupTop is still a cell index here }
      TmpB := $EA74;
      SoundRequest := 3;
      RequestSound;
      ShatterGroup;
      TmpA := Word(GroupBottom) shr 1 - TPRandom(GroupCols);
      Fl := Anims[10, 1];
      SpawnActor((TmpA mod MapWidth) shl 3, (TmpA div MapWidth) shl 3, 0, 0,
        Fl, 11, 8, 5);
      if TmpA = 1 then
        AnimationInit(Actors[ExtraCount + 2].Animation, Fl, Anims[10, 2],
          2, 1);
      RemoveGroup(G);
    end
    else
    begin
      with Groups[G] do
      begin
        VelocityY := VY;
        PhaseXBits := PX;
        PhaseYBits := PY;
        VelocityX := VX;
        FirstWordMapOffset := GroupLeft;
        LastWordMapOffset := GroupBottom;
        PreviousSpeedSum := TmpB;
      end;
      if Groups[G].SettleCounter = $5F then
      begin
        GroupTop := GroupTop shl 1;
        RemoveGroup(G);
      end;
      if G = 0 then
        Halt(0);
    end;
    Dec(G);
  until G = 0;
end;

{ 1568A  Energy bar of TmpB (0..100) pixels at screen offset 165 * 320 +
  TmpA: TmpB pixels of colour $0E, then 100 - TmpB of colour 1. }
procedure DrawEnergyBar; assembler;
asm
  mov cx, TmpB
  cmp cx, 100
  ja @Done
  mov ax, $A000
  mov es, ax
  mov ax, 165
  mov bx, 320
  mul bx
  mov di, ax
  add di, TmpA
  mov al, $0E
  rep stosb
  mov cx, 100
  sub cx, TmpB
  mov al, 1
  rep stosb
@Done:
end;

{ 156B6  Look at the 2x2 map cells TmpB, TmpB + 1, TmpB + 1 + MapWidth,
  TmpB + MapWidth (CX = 4..1). A projectile tile $75 sets TmpA := its cell
  and ByteTemp2 -= 2; while CX > the entry value of TmpA (signed), a solid
  tile 1..$4C also subtracts Flag from ByteTemp2. }
procedure ScanFootCells; assembler;
asm
  mov ByteTemp2, 0
  mov cx, 4
  mov bx, TmpA
  mov TmpA, 0
  mov es, MapSeg
  mov di, TmpB
@Look:
  mov al, byte ptr es:[di]
  cmp al, $75
  jne @NoShot
  mov TmpA, di
  dec ByteTemp2
  dec ByteTemp2
@NoShot:
  cmp cx, bx
  jle @Move
  cmp al, 1
  jb @Move
  cmp al, $4C
  ja @Move
  mov al, ByteTemp2
  sub al, Flag
  mov ByteTemp2, al
@Move:
  cmp cx, 4
  jne @Not4
  inc di
  jmp @Again
@Not4:
  cmp cx, 3
  jne @Not3
  add di, MapWidth
  jmp @Again
@Not3:
  cmp cx, 2
  jne @Again
  dec di
@Again:
  loop @Look
end;

{ 15715  Value := Value + Delta (modulo 256), then at most Limit. }
procedure WrappingAddClamp(var Value: Byte; Delta, Limit: Byte);
begin
  Inc(Value, Delta);
  if Value > Limit then
    Value := Limit;
end;
