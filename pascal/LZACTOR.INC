{ LZACTOR.INC -- main program, original linear 0x15740..0x17782: the tile
  replacement triggers and the per-frame actor update (players, monsters,
  thrown bombs, pick-ups, effects and the boss). Names are descriptive
  choices, not recovered identifiers. }

{ 15740 }
{ Switch trigger: plays sound 6 and, for the LAST tile replacement whose
  TriggerMapId equals Trigger (bit 15 ignored), replaces its FromTiles by its
  ToTiles in the map cells 1..LevelSize whose tag (bit 15 ignored) lies in
  FirstMapId..LastMapId. Cell 0 is never examined. }
procedure ReplaceTiles(Trigger: Word);
var
  I: Word;
  J: Word;              { also keeps TmpB across RequestSound }
  Found: Word;
  Tag: Word;
begin
  Trigger := Trigger and $7FFF;
  J := TmpB;
  TmpB := $27;
  SoundRequest := 6;
  RequestSound;
  TmpB := J;
  Found := 0;
  if ReplCount > 0 then
    for I := 1 to ReplCount do
      if Replacements[I].TriggerMapId = Trigger then
        Found := I;
  if Found > 0 then
    with Replacements[Found] do
      for I := 1 to LevelSize do
      begin
        Tag := MemW[MapBSeg : I * 2];
        Tag := Tag and $7FFF;
        if (Tag >= FirstMapId) and (Tag <= LastMapId) then
          for J := 1 to 4 do
            if (FromTiles[J] > 0) and (Mem[MapSeg : I] = FromTiles[J]) then
              Mem[MapSeg : I] := ToTiles[J];
      end;
end;

{ 16053 }
{ One frame of an actor: animation step; distances to the players, their
  touches and pick-ups; then by behaviour: 0/1 player (input, jumping,
  ducking, doors, bomb selection and throwing, tile pick-ups, projectile
  hits), 2 falling object, 3 walker, 4 homing flyer, 5 timed effect, 6 the
  boss; map collision, sub-pixel movement, monster hits, and the expiry of
  bombs (explosion, splinters), of defeated monsters (kind $C: random
  pick-up or puff) and of pick-up items. }
procedure UpdateActor(var Actor: TActor38);
type
  TCells = array[0..32766] of Word;
  PCells = ^TCells;
label
  Burst;
var
  Corner: Word;                 { bp-02 }
  DistX, DistY: Integer;        { bp-04, -06 to the nearer player }
  DistX2, DistY2: Integer;      { bp-08, -0A to player 2 }
  VX, VY: Integer;              { bp-0C, -0E }
  What: Byte;                   { bp-0F kind at entry }
  FracX, FracY: Byte;           { bp-10, -11 }
  N: Byte;                      { bp-12 scratch }
  Slot: Byte;                   { bp-13 render entry }
  CurFrame: Byte;               { bp-14 frame after the animation step }
  SquatFrame: Byte;             { bp-15 player frames }
  DuckFrame: Byte;              { bp-16 }
  StandFrame: Byte;             { bp-17 }
  LeftFirst, LeftLast: Byte;    { bp-18, -19 }
  RightFirst, RightLast: Byte;  { bp-1A, -1B }
  Pick: Byte;                   { bp-1C pick-up touched in this frame }
  Player: Byte;                 { bp-1D }
  FlagA, FlagB: Boolean;        { bp-1E, -1F }
  Turn: Boolean;                { bp-20 }
  Ground, Ceiling: Boolean;     { bp-21, -22 }
  WallLeft, WallRight: Boolean; { bp-23, -24 }
  StepLeft, StepRight: Boolean; { bp-25, -26 }
  Unused: Byte;                 { bp-27, never referenced }
  Cell: Word;                   { bp-2A map cell of the 4x4 block }
  X, Y: Word;                   { bp-2C, -2E }
  OldX: Word;                   { bp-30 }
  Behave: Byte;                 { bp-31 }
  Disp: ^TScoreDisplay45;       { bp-36 }

  { 15872 }
  { Kind $1F (moving part): adds the velocity of motion Index (bit 7:
    reversed) and damps by the motion's drag; drag $FF means the motion
    gives the position itself. }
  procedure FollowMotion(Index: Byte);
  var
    Sign: Word;
  begin
    Sign := 1;
    if Index > $80 then
    begin
      Dec(Index, $80);
      Sign := $FFFF;
    end;
    with Motions[Index] do
      if ModeOrDrag <> $FF then
      begin
        Inc(VX, ResultXBits * Sign);
        Inc(VY, ResultYBits * Sign);
        if Integer(Abs(VX)) > ModeOrDrag then
          if VX > 0 then
            Dec(VX, ModeOrDrag)
          else
            Inc(VX, ModeOrDrag);
        if Integer(Abs(VY)) > ModeOrDrag then
          if VY > 0 then
            Dec(VY, ModeOrDrag)
          else
            Inc(VY, ModeOrDrag);
      end
      else
      begin
        X := ResultXBits;
        Y := ResultYBits;
        VX := 0;
        VY := 0;
      end;
  end;

  { 15999 }
  { Door: moves the actor to the first map point tagged Id (bit 15
    ignored) with sound 4 and a flash; clears both players' down keys. }
  procedure Teleport(Id: Word);
  var
    I: Integer;
  begin
    Id := Id and $7FFF;
    I := 1;
    Keys[5] := 0;
    Keys[10] := 0;
    while PointCount >= I do
    begin
      with MapPoints[I] do
        if MapId = Id then
        begin
          X := XBits;
          Y := YBits;
          N := Anims[10, 1];
          TmpB := $1A;
          SoundRequest := 4;
          RequestSound;
          SpawnActor(X, Y, 0, 0, N, $B, 8, 5);
          if TmpA = 1 then
            AnimationInit(Actors[ExtraCount + 2].Animation, N, Anims[10, 2],
              2, 1);
          Exit;
        end;
      Inc(I);
    end;
  end;

  { 15A75 }
  { Shows sprite Sprite on the actor's render entry; the vertical origin
    becomes 16 - height. }
  procedure SetSprite(Sprite: Byte);
  begin
    with RenderEntries[Actor.SpriteSlot] do
    begin
      BitmapOffset := SpriteDefs[Sprite].BitmapOffset;
      Width := SpriteDefs[Sprite].Width;
      Height := SpriteDefs[Sprite].Height;
      Actor.OriginYAdjustment := 16 - Height;
    end;
  end;

  { 15AFD }
  { Tile pick-up at map cell GTemp. For the tiles $67..$72: ByteTemp :=
    tile, TmpA := its bonus sprite, TmpB := TmpB + its points; the cell is
    emptied (tile and tag 0), or gets tile $FF while its tag has bit 15;
    ByteTemp2 := 1 when the cell above has a tag 1..$7FFF. Otherwise
    ByteTemp, TmpA and ByteTemp2 are 0. }
  procedure Collect;
  begin
    asm
      mov byte ptr ByteTemp, 0
      mov word ptr TmpA, 0
      mov byte ptr ByteTemp2, 0
      mov es, MapSeg
      mov di, GTemp
      mov al, es:[di]
      cmp al, $66
      jle @NoBonus
      cmp al, $73
      jge @NoBonus
      mov ByteTemp, al
      sub al, $66
      dec al
      mov bl, al
      xor bh, bh
      shl bl, 1
      mov ax, word ptr [bx + BonusSprites]
      mov TmpA, ax
      mov ax, word ptr [bx + ScoreTable + 2]
      add TmpB, ax
      push di
      mov es, MapBSeg
      shl di, 1
      mov ax, es:[di]
      push ax
      sub di, RowBytes
      mov ax, es:[di]
      cmp ax, 0
      je @AboveDone
      cmp ax, $8000
      jae @AboveDone
      mov byte ptr ByteTemp2, 1
    @AboveDone:
      pop ax
      cmp ax, $8000
      jae @Tagged
      xor ax, ax
      add di, RowBytes
      mov es:[di], ax
      jmp @SetTile
    @Tagged:
      mov ah, $FF
    @SetTile:
      pop di
      mov es, MapSeg
      mov es:[di], ah
    @NoBonus:
    end ['AX', 'BX', 'DI'];
  end;

  { 15B86 }
  { Ground friction on VX. }
  procedure ApplyFriction;
  begin
    if Integer(Abs(VX)) < 43 then
      VX := 0
    else if VX < 0 then
      Inc(VX, 42)
    else
      Dec(VX, 42);
  end;

  { 15CB0 }
  { Behaviour 6, the boss: a block of W x H cells (Param0E) that collides
    with the map along its edges, hops at random every 29 frames, loses
    energy on the projectile tiles ($75) inside its block and is defeated
    when CounterOrLink wraps to $FF. }
  procedure UpdateBoss;
  label
    CeilingDone, GroundDone, LeftDone, RightDone;
  var
    W, H: Byte;
    T: Byte;
    P: Word;
    Row: Word;
    I, J: Word;

    { 15BCC }
    { Boss defeated: its parts (kind $1F with OriginId = its Param12) and
      the boss itself become falling debris (kind $E); trigger $3E8. }
    procedure BossDefeated;
    var
      K: Word;
      Tag: Word;
    begin
      Tag := Actor.Param12;
      for K := 1 to ExtraCount do
        with Actors[K + 2] do
          if (Kind = $1F) and (OriginId = Tag) then
          begin
            Kind := $E;
            CounterOrLink := TPRandom(10) + $28;
            Behavior := 2;
            Animation.Mode := 0;
          end;
      Actor.Behavior := 2;
      Actor.Kind := $E;
      Actor.CounterOrLink := $3C;
      Actor.Animation.Mode := 0;
      ReplaceTiles($3E8);
      TmpB := $3D;
      SoundRequest := $C;
      RequestSound;
    end;

  begin
    with Actor do
    begin
      W := Param0E and $FF;
      H := Param0E shr 8;
      Cell := (Y shr 3) * MapWidth + X shr 3;
      P := Cell - MapWidth;
      I := 1;
      Ceiling := False;
      while W >= I do
      begin
        T := PByteWindow(MapA)^[P];
        if (T >= 1) and (T <= $4C) then
        begin
          Ceiling := True;
          goto CeilingDone;
        end;
        Inc(P);
        Inc(I);
      end;
    CeilingDone:
      P := H * MapWidth + Cell;
      I := 1;
      Ground := False;
      while W >= I do
      begin
        T := PByteWindow(MapA)^[P];
        if (T >= 1) and (T <= $52) then
        begin
          Ground := True;
          goto GroundDone;
        end;
        Inc(P);
        Inc(I);
      end;
    GroundDone:
      P := Cell - 1;
      I := 1;
      WallLeft := False;
      while H >= I do
      begin
        T := PByteWindow(MapA)^[P];
        if (T >= 1) and (T <= $4C) then
        begin
          WallLeft := True;
          goto LeftDone;
        end;
        Inc(P, MapWidth);
        Inc(I);
      end;
    LeftDone:
      P := W + Cell;
      I := 1;
      WallRight := False;
      while H >= I do
      begin
        T := PByteWindow(MapA)^[P];
        if (T >= 1) and (T <= $4C) then
        begin
          WallRight := True;
          goto RightDone;
        end;
        Inc(P, MapWidth);
        Inc(I);
      end;
    RightDone:
      if FrameCount mod 29 = 0 then
      begin
        if (TPRandom(100) > $46) and ((FrameCount and 1) = 0) then
        begin
          TmpB := $69;
          SoundRequest := 4;
          RequestSound;
        end;
        if DistX > 0 then
          VX := TPRandom(800) + 150
        else
          VX := -150 - TPRandom(800);
        if Ground then
          VY := -TPRandom(1500) - 300;
      end;
      ByteTemp2 := 0;
      P := Cell;
      I := 1;
      while H >= I do
      begin
        Row := P;
        J := 1;
        while W >= J do
        begin
          T := PByteWindow(MapA)^[P];
          if T = $75 then
          begin
            Inc(ByteTemp2);
            TmpA := P;
          end;
          Inc(J, 2);
          Inc(P, 2);
        end;
        P := Row + MapWidth;
        Inc(I);
      end;
      if ByteTemp2 > 0 then
      begin
        FindProjectileAt;
        if ProjWeight[TmpB] > 1 then
          ByteTemp2 := ByteTemp2 * 2;
        T := Energy;
        if ByteTemp2 > T then
          Dec(Actor.CounterOrLink);
        Dec(Energy, ByteTemp2);
        SetSprite($2F);
      end;
      if CounterOrLink = $FF then
        BossDefeated;
    end;
    if not Ground then
      Inc(VY, $40);
    if (Ceiling and (VY < 0)) or (Ground and (VY > 0)) then
      VY := -(VY div 2);
    if (WallLeft and (VX < 0)) or (WallRight and (VX > 0)) then
      VX := -(VX div 2);
  end;

begin
  { animation step }
  Slot := Actor.SpriteSlot;
  with Actor.Animation do
    if Mode > 0 then
    begin
      Inc(Counter);
      if Counter > Delay then
      begin
        Counter := 0;
        Frame := Frame + Step;
        if Mode <> 2 then
        begin
          if Frame > LastFrame then
          begin
            Frame := FirstFrame;
            if Mode = 3 then
              Move(Actor.SavedAnimation, Actor.Animation, 7);
          end;
        end
        else if (Frame >= LastFrame) or (Frame <= FirstFrame) then
          Step := -Step;
        RenderEntries[Slot].BitmapOffset := SpriteDefs[Frame].BitmapOffset;
      end;
    end;
  Behave := Actor.Behavior;
  CurFrame := Actor.Animation.Frame;
  { the players: behaviour 1 is player 2 and is handled as behaviour 0 }
  case Behave of
    0:
      begin
        CurKeys[1] := Keys[1];
        CurKeys[2] := Keys[2];
        CurKeys[3] := Keys[3];
        CurKeys[4] := Keys[4];
        CurKeys[5] := Keys[5];
        SquatFrame := $12;
        DuckFrame := $13;
        StandFrame := 1;
        LeftFirst := Anims[6, 1];
        LeftLast := Anims[6, 2];
        RightFirst := Anims[5, 1];
        RightLast := Anims[5, 2];
        Disp := @ScoreDisp1;
        Pick := Pickup[1];
        Player := 1;
        BombSwitchPtr := @BombSwitch[1];
      end;
    1:
      begin
        CurKeys[1] := Keys[6];
        CurKeys[2] := Keys[7];
        CurKeys[3] := Keys[8];
        CurKeys[4] := Keys[9];
        CurKeys[5] := Keys[10];
        Behave := 0;
        SquatFrame := $25;
        DuckFrame := $26;
        StandFrame := $14;
        LeftFirst := Anims[8, 1];
        LeftLast := Anims[8, 2];
        RightFirst := Anims[7, 1];
        RightLast := Anims[7, 2];
        Disp := @ScoreDisp2;
        Pick := Pickup[2];
        Player := 2;
        BombSwitchPtr := @BombSwitch[2];
      end;
  end;
  with RenderEntries[Slot] do
  begin
    X := XBits;
    Y := YBits;
  end;
  with Actor do
  begin
    FracX := FractionX;
    FracY := FractionY;
    VX := VelocityX;
    VY := VelocityY;
    Y := Y - OriginYAdjustment;
    What := Kind;
  end;
  OldX := X;
  { monsters and pick-ups: distances to the players, touches, pick-ups.
    DistY (DistY2) stays unassigned while player 1 (2) is not active. }
  if ((What >= 1) and (What <= 8)) or (What > $12) then
  begin
    if PlayerState[1] = 1 then
      with RenderEntries[Actors[1].SpriteSlot] do
      begin
        DistX := XBits - X;
        DistY := YBits - Y;
      end
    else
      DistX := 6000;
    if PlayerState[2] = 1 then
      with RenderEntries[Actors[2].SpriteSlot] do
      begin
        DistX2 := XBits - X;
        DistY2 := YBits - Y;
      end
    else
      DistX2 := 6000;
    if What = $1F then
      with Actor do
      begin
        CounterOrLink := $FA;
        if (Param0E and $FF) > 0 then
        begin
          FollowMotion(Param0E and $FF);
          if (Param0E shr 8) > 0 then
          begin
            FollowMotion(Param0E shr 8);
            if (Param10 and $FF) > 0 then
              FollowMotion(Param10 and $FF);
          end;
        end;
      end;
    if (Integer(Abs(DistX)) < 10) and (Integer(Abs(DistY)) < 10) then
      if ((What >= 1) and (What <= 8)) or (What > $1D) then
        Inc(Touches[1])
      else if Pickup[1] = 0 then
      begin
        Pickup[1] := What - $12;
        with Actor do
        begin
          Behavior := 5;
          Kind := $B;
          SetSprite(PickupSprite[Pickup[1]]);
          CounterOrLink := $1A;
          Animation.Mode := 0;
        end;
        VX := 0;
        VY := (TPRandom(3) + 1) * -100;
      end;
    if (Integer(Abs(DistX2)) < 10) and (Integer(Abs(DistY2)) < 10) then
      if ((What >= 1) and (What <= 8)) or (What > $1D) then
        Inc(Touches[2])
      else if Pickup[2] = 0 then
      begin
        Pickup[2] := What - $12;
        with Actor do
        begin
          Behavior := 5;
          Kind := $B;
          SetSprite(PickupSprite[Pickup[2]]);
          CounterOrLink := $1A;
          Animation.Mode := 0;
        end;
        VX := 0;
        VY := (TPRandom(3) + 1) * -100;
      end;
    { 16-bit Abs, the sums in 32 bits }
    if LongInt(Integer(Abs(DistX))) + Integer(Abs(DistY)) >
       LongInt(Integer(Abs(DistX2))) + Integer(Abs(DistY2)) then
    begin
      DistX := DistX2;
      DistY := DistY2;
    end;
  end;
  if Behave = 6 then
    UpdateBoss
  else
  begin
    { Around[4 * row + col]: the 4 x 4 tile codes from one row above the
      actor's top row and one column left of (X + 4) shr 3 }
    asm
      les di, MapA
      mov ax, X
      add ax, 4
      shr ax, 1
      shr ax, 1
      shr ax, 1
      mov cx, ax
      mov ax, Y
      shr ax, 1
      shr ax, 1
      shr ax, 1
      dec ax
      dec cx
      mov bx, MapWidth
      mul bx
      add ax, cx
      add di, ax
      mov Cell, di
      xor bx, bx
      mov cx, 4
    @AroundRow:
      push cx
      push di
      mov cx, 4
    @AroundCol:
      mov al, es:[di]
      mov byte ptr [bx + Around], al
      inc bx
      inc di
      loop @AroundCol
      pop di
      pop cx
      add di, MapWidth
      loop @AroundRow
    end ['AX', 'BX', 'CX', 'DX', 'DI'];
    if Behave = 5 then
    begin
      Dec(Actor.CounterOrLink, FrameCount and 1);
      if Actor.CounterOrLink = 0 then
      begin
        RemoveActor(GI);
        Dec(GI);
        if What = $A then
          Dec(KindTenCount);
        Exit;
      end;
    end
    else
    begin
      if What = $D then
      begin
        if Around[13] in [1..$52] then
          Ground := True
        else
          Ground := False;
        if Around[8] in [1..$4C] then
          WallLeft := True
        else
          WallLeft := False;
        if Around[10] in [1..$4C] then
          WallRight := True
        else
          WallRight := False;
        if Around[5] in [1..$4C] then
          Ceiling := True
        else
          Ceiling := False;
      end
      else
      begin
        if Around[13] in [1..$52] then
          Ground := True
        else if Around[14] in [1..$52] then
          Ground := True
        else
          Ground := False;
        if Around[4] in [1..$4C] then
          WallLeft := True
        else if Around[8] in [1..$4C] then
          WallLeft := True
        else
          WallLeft := False;
        if Around[7] in [1..$4C] then
          WallRight := True
        else if Around[11] in [1..$4C] then
          WallRight := True
        else
          WallRight := False;
        if Around[1] in [1..$4C] then
          Ceiling := True
        else if Around[2] in [1..$4C] then
          Ceiling := True
        else
          Ceiling := False;
      end;
      case Behave of
        0:
          { player }
          begin
            StepLeft := WallLeft and not (Around[4] in [1..$4C]);
            StepRight := WallRight and not (Around[7] in [1..$4C]);
            if not Ground or (VY < 0) then
            begin
              Inc(VY, $40);
              if VY > $7FF then
                VY := $7FF;
            end
            else if VY > 0 then
            begin
              Y := Y and $FFF8;
              if VY > $640 then
              begin
                { hard landing: squat, then back to the saved animation }
                if Actor.Animation.FirstFrame <> SquatFrame then
                begin
                  with Actor do
                    if Animation.Mode = 0 then
                      AnimationInit(SavedAnimation, StandFrame, StandFrame,
                        2, 3)
                    else
                      Move(Animation, SavedAnimation, 7);
                  AnimationInit(Actor.Animation, SquatFrame - 1, SquatFrame,
                    3, 3);
                  RenderEntries[Slot].BitmapOffset :=
                    SpriteDefs[SquatFrame].BitmapOffset;
                end;
                VY := (-VY) div 4;
              end
              else
                VY := 0;
            end;
            { left + right together: request the next bomb kind }
            if CurKeys[2] + CurKeys[3] = 2 then
            begin
              CurKeys[2] := 0;
              CurKeys[3] := 0;
              Inc(Byte(BombSwitchPtr^));
            end
            else
            begin
              if Byte(BombSwitchPtr^) >= 5 then
              begin
                TmpB := $24;
                SoundRequest := 2;
                RequestSound;
                BombIconDirty[Player] := 2;
                ByteTemp := 0;
                repeat
                  N := SelBomb[Player] mod 4 + 1;
                  SelBomb[Player] := N;
                  Inc(ByteTemp);
                until (Bombs[Player, N] > 0) or (ByteTemp > 4);
                if ByteTemp > 4 then
                  Actor.Energy := $FE;
              end;
              Byte(BombSwitchPtr^) := 0;
              if CurKeys[5] + CurKeys[1] = 2 then
              begin
                CurKeys[5] := 0;
                CurKeys[1] := 0;
              end;
              { down: door, spring, or duck }
              if CurKeys[5] = 1 then
                if Around[13] = $45 then
                  Teleport(PCells(MapB)^[MapWidth * 3 + Cell + 1])
                else if (Around[13] = $27) and not Ceiling then
                begin
                  VY := -2000;
                  TmpB := $35;
                  SoundRequest := 5;
                  RequestSound;
                  SpawnActor(X + 4, Y + $D, 0, -200, $5B, $B, 5, 5);
                  if TmpA = 1 then
                    Actors[ExtraCount + 2].Animation.Mode := 0;
                end
                else if Ground and (VY = 0) and (Actor.Param0E = 0) then
                begin
                  if Around[13] in [1..$4C] then
                    FlagA := True
                  else if Around[14] in [1..$4C] then
                    FlagA := True
                  else
                    FlagA := False;
                  if not FlagA then
                  begin
                    if Actor.Animation.FirstFrame <> SquatFrame then
                    begin
                      with Actor do
                        if Animation.Mode = 0 then
                          AnimationInit(SavedAnimation, StandFrame,
                            StandFrame, 2, 3)
                        else
                          Move(Animation, SavedAnimation, 7);
                      AnimationInit(Actor.Animation, SquatFrame - 1,
                        DuckFrame, 3, 3);
                      RenderEntries[Slot].BitmapOffset :=
                        SpriteDefs[SquatFrame].BitmapOffset;
                    end;
                    Actor.Param0E := 4;
                  end
                  else
                    CurKeys[5] := 0;
                end
                else
                  CurKeys[5] := 0;
              { sinking through a platform }
              if Actor.Param0E > 0 then
              begin
                Inc(Y, 2);
                Dec(Actor.Param0E);
                Actor.CounterOrLink := $F8;
              end;
              if CurKeys[2] = 1 then
              begin
                if StepLeft then
                begin
                  VY := -500;
                  WallLeft := False;
                  VX := -250;
                end;
                if (CurFrame < LeftFirst) or (CurFrame > LeftLast) then
                begin
                  AnimationInit(Actor.Animation, LeftFirst, LeftLast, 0, 1);
                  Actor.CounterOrLink := 0;
                end;
                if VX > -1024 then
                  Dec(VX, $40);
              end;
              if CurKeys[3] = 1 then
              begin
                if StepRight then
                begin
                  VY := -500;
                  WallRight := False;
                  VX := 250;
                end;
                if (CurFrame > RightLast) or (CurFrame < RightFirst) then
                begin
                  AnimationInit(Actor.Animation, RightFirst, RightLast, 0, 1);
                  Actor.CounterOrLink := 0;
                end;
                if VX < $400 then
                  Inc(VX, $40);
              end;
            end;
            if Ground then
            begin
              if (CurKeys[1] = 1) and (VY = 0) then
                VY := -848;
              if CurKeys[3] + CurKeys[2] = 0 then
                ApplyFriction;
            end;
            Actor.Animation.Delay := 4 - Integer(Abs(VX)) div $100;
            { idle: back to the standing frame after 5 frames }
            if CurKeys[2] + CurKeys[3] + CurKeys[5] = 0 then
            begin
              Inc(Actor.CounterOrLink);
              if Actor.CounterOrLink = 5 then
                with Actor.Animation do
                begin
                  Mode := 0;
                  Frame := 1;
                  RenderEntries[Slot].BitmapOffset :=
                    SpriteDefs[StandFrame].BitmapOffset;
                end;
            end;
            { fire: throw a bomb of the selected kind }
            if CurKeys[4] = 1 then
            begin
              N := SelBomb[Player];
              if Bombs[Player, N] > 0 then
              begin
                if N <> 4 then
                  ByteTemp := N * 10 + 10
                else
                  ByteTemp := $C8;
                SpawnActor(X, Y, VX * 3 div 2, VY - 500, N + $3A - 1, N + $C,
                  ByteTemp, 2);
                if TmpA = 1 then
                begin
                  AnimationInit(Actors[ExtraCount + 2].Animation, 0, 0, 0, 0);
                  Dec(Bombs[Player, SelBomb[Player]]);
                  BombIconDirty[Player] := 1;
                end;
                Keys[4] := 0;
                Keys[9] := 0;
              end;
            end;
            { tile pick-ups in the four cells of the body; Second2 serves
              as the "big bonus" flag here }
            TmpB := 0;
            GTemp := Cell + MapWidth;
            Second2 := 0;
            for Corner := 1 to 4 do
            begin
              case Corner of
                1:
                  begin
                    Inc(GTemp);
                    MoveVX := -2;
                    MoveVY := -2;
                  end;
                2:
                  begin
                    Inc(GTemp);
                    MoveVX := 10;
                    MoveVY := -2;
                  end;
                3:
                  begin
                    Inc(GTemp, MapWidth);
                    MoveVX := 10;
                    MoveVY := 10;
                  end;
                4:
                  begin
                    Dec(GTemp);
                    MoveVX := -2;
                    MoveVY := 10;
                  end;
              end;
              if PByteWindow(MapA)^[GTemp] = $72 then
                ReplaceTiles(PCells(MapB)^[GTemp]);
              Collect;
              if ByteTemp2 = 1 then
                SetCellMoving(GTemp - MapWidth, 0, 0, N);
              if ByteTemp = CountedTile then
                Inc(BonusCollected);
              if ByteTemp > $6C then
                Second2 := 1;
              if (TmpA > 0) and (KindTenCount < $E) then
              begin
                SpawnActor(X + MoveVX, Y + MoveVY, 0, -40 - TPRandom(200),
                  TmpA, $A, $C, 5);
                AnimationInit(Actors[ExtraCount + 2].Animation, 0, 0, 0, 0);
                if TmpA = 1 then
                  Inc(KindTenCount);
              end;
            end;
            if TmpB > 0 then
            begin
              Disp^.DisplayState := 0;
              Inc(Disp^.Score, TmpB);
              TmpB := 0;
              if Second2 > 0 then
                TmpB := $12;
              SoundRequest := 3;
              RequestSound;
            end;
            { the pick-up item touched in this frame }
            if Pick > 0 then
            begin
              case Pick of
                2: Actor.Energy := $64;
                3:
                  begin
                    Inc(Actor.Energy, $21);
                    if Actor.Energy > $64 then
                      Actor.Energy := $64;
                  end;
                4: BonusRain := $2E;
                5:
                  begin
                    Bombs[Player, 1] := $C8;
                    WrappingAddClamp(Bombs[Player, 2], TPRandom(10) + 1, $63);
                    WrappingAddClamp(Bombs[Player, 3], TPRandom(4) + 1, $63);
                    BombIconDirty[Player] := 1;
                  end;
                6:
                  begin
                    Bombs[Player, 1] := $C8;
                    WrappingAddClamp(Bombs[Player, 2], TPRandom($D) + 1, $63);
                    WrappingAddClamp(Bombs[Player, 3], TPRandom(5) + 2, $63);
                    WrappingAddClamp(Bombs[Player, 4], TPRandom(2) + 1, $63);
                    BombIconDirty[Player] := 1;
                  end;
              end;
              Disp^.DisplayState := 0;
              Inc(Disp^.Score, PickupScore[Pick]);
              TmpB := 8;
              SoundRequest := 5;
              RequestSound;
            end;
            { hit by a projectile below the body }
            TmpB := Cell + MapWidth + 1;
            TmpA := 2;
            Flag := 2;
            ScanFootCells;
            if TmpA <> 0 then
            begin
              FindProjectileAt;
              with Debris[TmpB] do
              begin
                VX := VelocityX * 8;
                VY := VelocityY * 8;
              end;
              if ProjWeight[TmpB] > 1 then
                Dec(ByteTemp2, ProjWeight[TmpB] div 10);
            end;
            Actor.Energy := Actor.Energy + ByteTemp2;
          end;
        2:
          { falling object }
          begin
            if not Ground or (VY < 0) then
            begin
              Inc(VY, $40);
              if VY > $7FF then
                VY := $7FF;
            end
            else if VY > 0 then
            begin
              VY := 0;
              Y := Y and $FFF8;
            end;
            if Ground then
              ApplyFriction;
          end;
        4:
          { homing flyer: every Param0E frames aims at the nearer player
            with speed Param10 when closer than Param12, else at random }
          begin
            if Around[13] in [1..$4C] then
              FlagA := True
            else if Around[14] in [1..$4C] then
              FlagA := True
            else
              FlagA := False;
            if FlagA and Ceiling then
              VY := 0;
            if FlagA and (VY > 0) then
              VY := (-VY) div 2;
            GTemp := Actor.Param0E;
            TmpB := Actor.Param10;
            TmpA := Actor.Param12;
            if FrameCount mod GTemp = 0 then
              { 16-bit sum, compared in 32 bits with the Word }
              if Integer(Abs(DistX) + Abs(DistY)) < LongInt(TmpA) then
                AimVelocity(DistX, DistY, TmpB, VX, VY)
              else
              begin
                VX := TPRandom(TmpB * 2) - TmpB;
                VY := TPRandom(TmpB * 2) - TmpB;
              end;
          end;
        3:
          { walker: speed Param0E, turns at walls and at edges }
          begin
            Turn := WallLeft or WallRight;
            if not Ground or (VY < 0) then
            begin
              Inc(VY, $40);
              if VY > $7FF then
                VY := $7FF;
            end
            else if VY > 0 then
            begin
              VY := 0;
              Y := Y and $FFF8;
              Turn := True;
            end;
            if Ground then
            begin
              if VX <> 0 then
              begin
                if LongInt(Integer(Abs(VX))) <> LongInt(Actor.Param0E) then
                begin
                  VX := Integer(Abs(VX)) div VX * Actor.Param0E;
                  Turn := True;
                end;
              end
              else
                VX := Actor.Param0E;
              { FlagA, FlagB: no floor below-left, below-right }
              FlagA := not (Around[12] in [1..$52]);
              FlagB := not (Around[15] in [1..$52]);
              if FlagA or FlagB then
                Turn := True;
              if not FlagA or not FlagB then
                if VX < 0 then
                begin
                  X := OldX;
                  if FlagA then
                    VX := -VX;
                end
                else
                begin
                  X := OldX;
                  if FlagB then
                    VX := -VX;
                end;
            end;
            if Turn then
              if VX < 0 then
              begin
                N := Actor.AnimationSelectorA;
                with Actor.Animation do
                begin
                  FirstFrame := Anims[N, 1];
                  LastFrame := Anims[N, 2];
                  Frame := FirstFrame;
                end;
              end
              else if VX > 0 then
              begin
                N := Actor.AnimationSelectorB;
                with Actor.Animation do
                begin
                  FirstFrame := Anims[N, 1];
                  LastFrame := Anims[N, 2];
                  Frame := FirstFrame;
                end;
              end;
          end;
      end;
      { kind 4 also collects the bonus tiles under it }
      if What = 4 then
      begin
        GTemp := Cell + MapWidth;
        TmpB := 0;
        Collect;
        Inc(TmpB, TmpA);
        if ByteTemp2 = 1 then
          SetCellMoving(GTemp - MapWidth, 0, 0, N);
        Inc(GTemp, MapWidth + 1);
        Collect;
        Inc(TmpB, TmpA);
        if TmpB > 0 then
        begin
          SoundRequest := 1;
          TmpB := $21;
          RequestSound;
        end;
      end;
      if Ceiling and (VY < 0) then
        VY := 1;
      if WallLeft and WallRight then
        VX := 0;
      if (WallLeft and (VX < 0)) or (WallRight and (VX > 0)) then
      begin
        VX := (-VX) div 2;
        if VX < 0 then
          Dec(X)
        else
          Inc(X);
      end;
    end;
  end;
  { position += velocity / 256, the low bytes kept as fractions }
  asm
    mov ax, VY
    xor bh, bh
    cmp ax, 0
    jge @PositiveY
    mov bh, $FF
  @PositiveY:
    mov bl, FracY
    add bl, al
    mov FracY, bl
    mov al, ah
    mov ah, bh
    adc Y, ax
    mov ax, VX
    xor bh, bh
    cmp ax, 0
    jge @PositiveX
    mov bh, $FF
  @PositiveX:
    mov bl, FracY
    mov bl, FracX
    add bl, al
    mov FracX, bl
    mov al, ah
    mov ah, bh
    adc X, ax
  end ['AX', 'BX'];
  { monsters hit by projectiles }
  if (What >= 1) and (What <= 8) and (What <> $1F) then
  begin
    TmpA := 0;
    TmpB := Cell + MapWidth + 1;
    Flag := 1;
    ScanFootCells;
    if ByteTemp2 <> 0 then
    begin
      if VX > 0 then
        N := 2
      else
        N := 1;
      SetSprite(Anims[What + $10, N]);
      with Actor.Animation do
        Counter := Delay - 4;
      if Actor.Energy + ByteTemp2 < 0 then
      begin
        Actor.Animation.Mode := 0;
        with Actor do
        begin
          Behavior := 2;
          Kind := $C;
          CounterOrLink := $19;
          if OriginId > 0 then
            Inc(Spawns[OriginId].AvailableCount);
        end;
      end
      else
        Actor.Energy := Actor.Energy + ByteTemp2;
    end;
  end;
  Y := Y + Actor.OriginYAdjustment;
  with RenderEntries[Slot] do
  begin
    XBits := X;
    YBits := Y;
  end;
  with Actor do
  begin
    FractionX := FracX;
    FractionY := FracY;
    VelocityX := VX;
    VelocityY := VY;
  end;
  { kinds $C..$1D count down: bombs ($D..$12) explode and become a puff, a
    defeated monster ($C) becomes a random pick-up item or a puff, both
    throw splinters; the other kinds vanish in a puff }
  if (What >= $C) and (Behave <> 5) and (What < $1E) then
  begin
    Dec(Actor.CounterOrLink, FrameCount and 1);
    if (Actor.CounterOrLink = 0) or (Actor.CounterOrLink = $FF) then
    begin
      if (What > $C) and (What < $13) then
        Explode((Y shr 3) * MapWidth + X shr 3, KindBomb[What - $C]);
      if What <> $C then
      begin
        { the original has a "with Actor do" around this block and jumps
          into it; it is written out here so that the goto is safe }
      Burst:
        Actor.Behavior := 5;
        Actor.VelocityX := 0;
        Actor.VelocityY := 0;
        Actor.Kind := 0;
        Actor.CounterOrLink := $12;
        if What < $13 then
          N := Anims[9, 1]
        else
          N := Anims[10, 1];
        SetSprite(N);
        AnimationInit(Actor.Animation, N, Anims[10, 2], 2, 1);
      end
      else
      begin
        ByteTemp := TPRandom(100);
        TmpB := TPRandom(20) + $EA74;
        SoundRequest := 4;
        RequestSound;
        if ByteTemp < PickupChance[0] then
          goto Burst
        else
        begin
          N := 1;
          while PickupChance[N] < ByteTemp do
            Inc(N);
        end;
        Dec(N);
        SetSprite(N + $3E);
        with Actor do
        begin
          Kind := N + $13;
          CounterOrLink := $64;
          Animation.Mode := 0;
          Dec(VelocityY, $C8);
        end;
      end;
      if What < $13 then
      begin
        N := What - $C;
        if N <= 0 then
          N := 1;
        for What := 1 to Byte(N + 1) do
        begin
          SpawnActor(X, Y, TPRandom(600) - 300, TPRandom(600) - 300, $D, $B,
            $F, 5);
          if TmpA = 1 then
            AnimationInit(Actors[ExtraCount + 2].Animation, Anims[9, 1],
              Anims[10, 2], 2, 2);
        end;
      end;
    end;
  end;
end;
