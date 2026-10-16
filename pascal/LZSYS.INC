{ LZSYS.INC -- main program, original linear 0x10000..0x11155: checksum,
  shutdown, picture/palette/sound/level loading, small screen helpers and the
  INT 1Ch / INT 9 handlers. }

{ 10000 Alternating sum of Count bytes: odd positions are added, even ones
  subtracted. The original receives the buffer as a segment word and an offset
  word (the caller pushes DSeg and Ofs(Texts)), reads Mem[Seg:Ofs] and
  increments the offset parameter itself; the offset copy below does that. }
procedure AlternatingChecksum(var Source; var Accumulator: Word; Count: Word);
var
  I, Sample, Previous: Word;
  Offset: Word;
begin
  Offset := Ofs(Source);
  for I := 1 to Count do
  begin
    Sample := Mem[Seg(Source):Offset];
    if I and 1 = 1 then
      Inc(Accumulator, Sample)
    else
      Dec(Accumulator, Sample);
    Inc(Offset);
    Previous := Sample;
  end;
end;

{ 100A3 Restores the interrupt vectors, the keyboard repeat rate and text
  mode, prints a message for codes 1 and 2 and ends the program. }
procedure Shutdown(Code: Byte);
begin
  SetIntVec(9, OldInt9);
  SetIntVec($1C, OldInt1C);
  TPNoSound;
  asm
    mov al, $F3
    out $60, al
  end;
  TPDelay(200);
  asm
    mov al, $47
    out $60, al
    mov ax, 3
    int $10
  end;
  case Code of
    1: WriteLn('Sorry, errors in data. Files corrupted.');
    2: WriteLn('Sorry, low memory.');
  end;
  CheckBreak := SavedBreak;
  Halt(0);
end;

{ DrawWaveBands calls FillBox before its body (107C5). }
procedure FillBox(X, Y, W, H: Word; Colour: Byte); forward;

{ 101FC Clears the screen and fills it with wavy bands of colours $B0..$B6
  (the code at 102D1..1030A is the tail of this procedure). }
procedure DrawWaveBands;
const
  WaveStep: TReal = (W0: $297B; W1: $8F5C; W2: $75C2);    { 0.03 }
  WaveHeight: TReal = (W0: $0084; W1: $0000; W2: $0000);  { 8.0 }
var
  Y, Unused, Step, Swing: Integer;
  I, P, Band: Word;

  { 10139 Random gradient for the seven band colours. }
  procedure RandomBandColours;
  var
    Colour: Byte;
    R0, G0, B0, DR, DG, DB: Shortint;
  begin
    R0 := TPRandom(20);
    G0 := TPRandom(20);
    B0 := TPRandom(20);
    DR := TPRandom(30);
    DG := TPRandom(30);
    DB := TPRandom(30);
    for Colour := $B0 to $B6 do
      SetRGB(Colour, R0 + (Colour - $B0) * DR div 7,
             G0 + (Colour - $B0) * DG div 7, B0 + (Colour - $B0) * DB div 7);
  end;

begin
  FillBox(0, 0, 320, 200, 0);
  P := 0;
  Band := 0;
  Y := 0;
  Unused := 0;
  Step := TPRandom(80) + 1;
  Swing := TPRandom(80) + 1;
  RandomBandColours;
  for I := 0 to 63999 do
  begin
    Inc(Y, Step);
    if Y > 100 then
    begin
      Dec(Y, 100);
      Inc(Band);
    end;
    if I mod 320 = 0 then
    begin
      { Swing is widened (CWD) and added to the LongInt Trunc result; Inc
        adds only the low word. }
      Inc(Y, Integer(Swing + RTrunc(RMul(SinTable[RTrunc(RMul(RFromLong(I),
        WaveStep)) mod 128], WaveHeight))));
      if Y > 100 then
      begin
        Dec(Y, 100);
        Inc(Band);
      end;
    end;
    Mem[$A000:P] := Band mod 7 + $B0;
    Inc(P);
  end;
end;

{ 1030B Shows a packed full-screen picture: 768 palette bytes, a packed
  length and the packed pixels (unpacked straight into video memory), faded
  in over 64 steps; a key after step 2 jumps to full brightness. Colour 10
  is never faded. }
procedure FadeInPicture(Name: string);
var
  F: file;
  Dest: PByteWindow;
  Size: Word;
  Pal: array[0..768] of Byte;
  Faded: array[0..768] of Byte;
  FadedPtr: Pointer;
  Source: PByteWindow;
  Fade, J: Integer;
  Unused: Integer;
begin
  {$I-}
  Assign(F, Name);
  Reset(F, 1);
  BlockRead(F, Pal, 768);
  Source := Screen;
  BlockRead(F, Size, 2);
  BlockRead(F, Source^, Size);
  Close(F);
  {$I+}
  if IOResult <> 0 then
    Shutdown(1);
  Dest := Ptr($A000, 0);
  { The original assigns the FOR control variable inside the loop. }
  for Fade := 0 to 63 do
  begin
    if TPKeyPressed and (Fade > 2) then
    begin
      LastKey := TPReadKey;
      Fade := 63;
    end;
    { Index 768 is one past the 768 bytes read (uninitialised). }
    for J := 0 to 768 do
      Faded[J] := Pal[J] * Fade div 63;
    for J := 30 to 32 do
      Faded[J] := Pal[J];
    FadedPtr := @Faded;
    asm
      les di, FadedPtr
      mov dx, di
      mov cx, $100
      mov bx, 0
      mov ax, $1012
      int $10
    end;
    if Fade = 0 then
      LegacyExpandUnsafe(Source, 22, Dest, 64000);
    TPDelay(22);
  end;
end;

{ 10495 Starts a palette flash of DAC colour Colour from Start towards Goal,
  reusing the slot already flashing that colour; at most two slots. }
procedure StartFlash(Colour: Byte; Start, Goal: TRGB3);
var
  Slot, I: Byte;
begin
  Slot := FlashCount + 1;
  if FlashCount > 0 then
    for I := 1 to FlashCount do
      if FlashColour[I] = Colour then
        Slot := I;
  if Slot < 3 then
  begin
    if Slot > FlashCount then
      Inc(FlashCount);
    FlashColour[Slot] := Colour;
    FlashNow[Slot] := Start;
    FlashGoal[Slot] := Goal;
  end;
end;

{ 1056B Puts the actor's sprite on the first map point whose player flags
  match Mask, or at (17, 17) if there is none. }
procedure PlaceAtStartPoint(var Actor: TActor38; Mask: Byte);
var
  I: Integer;
  Found: Boolean;
begin
  I := 1;
  Found := False;
  Actor.CounterOrLink := 0;
  while (I <= PointCount) and not Found do
  begin
    with MapPoints[I] do
      if PlayerFlags and Mask > 0 then
      begin
        with RenderEntries[Actor.SpriteSlot] do
        begin
          XBits := MapPoints[I].XBits;
          YBits := MapPoints[I].YBits;
        end;
        Found := True;
      end;
    Inc(I);
  end;
  if not Found then
    with RenderEntries[Actor.SpriteSlot] do
    begin
      XBits := 17;
      YBits := 17;
    end;
end;

{ 10630 PROEFS.SON: a count, then 6 bytes per sound step. }
procedure LoadSounds;
var
  F: file;
  Count: Word;
begin
  {$I-}
  Assign(F, 'proefs.son');
  Reset(F, 1);
  BlockRead(F, Count, 2);
  BlockRead(F, Sounds^, Word(Count * 6));
  Close(F);
  {$I+}
  if IOResult <> 0 then
    Shutdown(1);
end;

{ 106AB Store order 0, 1, 2, 4, 5, 6, 3 as in the original. }
procedure AnimationInit(var Anim: TAnimation7; StartFrame, EndFrame, Ticks, AnimMode: Byte);
begin
  with Anim do
  begin
    Frame := StartFrame;
    FirstFrame := StartFrame;
    LastFrame := EndFrame;
    Delay := Ticks;
    Mode := AnimMode;
    Step := 1;
    Counter := Ticks;
  end;
end;

{ 10709 Loads 768 palette bytes and sets all 256 DAC colours. }
procedure LoadPalette(Name: string);
var
  F: file;
  Pal: array[0..768] of Byte;
  PalPtr: Pointer;
begin
  {$I-}
  Assign(F, Name);
  Reset(F, 1);
  BlockRead(F, Pal, 768);
  Close(F);
  {$I+}
  if IOResult <> 0 then
    Shutdown(1);
  PalPtr := @Pal;
  asm
    les di, PalPtr
    mov dx, di
    mov cx, $100
    mov bx, 0
    mov ax, $1012
    int $10
  end;
end;

{ 1079D DAC colours $E6..$EB: red rising by 7 from FireLevel (back to 20
  when it passes 63 as a signed byte), green and blue 0. }
procedure SetFirePalette; assembler;
asm
  mov cx, 6
  mov bl, $E6
  mov al, FireLevel
  xor ah, ah
  mov dx, $3C8
@Next:
  xchg bl, al
  out dx, al
  inc dx
  xchg bl, al
  out dx, al
  xchg al, ah
  out dx, al
  out dx, al
  xchg al, ah
  dec dx
  add al, 7
  cmp al, $3F
  jle @Keep
  mov al, $14
@Keep:
  inc bl
  loop @Next
end;

{ 107C5 Fills a W x H box on the mode 13h screen (H = 0 runs 65536 rows,
  as the LOOP instruction does). }
procedure FillBox(X, Y, W, H: Word; Colour: Byte);
begin
  asm
    mov ax, $A000
    mov es, ax
    mov ax, Y
    mov bx, 320
    mul bx
    add ax, X
    mov di, ax
    mov al, Colour
    mov cx, H
  @Row:
    push cx
    push di
    mov cx, W
    rep stosb
    pop di
    add di, 320
    pop cx
    loop @Row
  end;
end;

{ 107FA A box in colour Light with a Dark box one pixel inside it. }
procedure DrawBevelBox(X, Y, W, H: Word; Light, Dark: Byte);
begin
  FillBox(X, Y, W, H, Light);
  FillBox(Word(X + 1), Word(Y + 1), Word(W - 2), Word(H - 2), Dark);
end;

{ 10838 Sets the view width in tiles and the geometry derived from it. All
  arithmetic is unsigned 16-bit. }
procedure SetViewWidth(TileCount: Word);
begin
  ViewTiles := TileCount;
  ScreenSkip := Word(320 - ViewTiles * 8);
  ViewWidth := Word(ViewTiles * 8);
  CamMaxX := Integer((MapWidth - ViewTiles) * 8);
  ViewCenterX := Word(Word(ViewTiles - 2) div 2 * 8);
  ViewCells := ViewCenterX div 8;
  ScreenOfs := Word(320 - (ViewTiles - 1) * 8) div 2;
  BackgroundSkip := Word(BackgroundPitch - ViewWidth);
end;

{ 108A5 Loads an object file (the caller passes 'gran.mst' on level 7): a
  count, the actor records (stored from Actors[ExtraCount + 3] on), one sprite
  number per actor, a position per actor (relative to OfsX/OfsY), then a
  count and that many motion records. The file is never closed. }
procedure LoadExtraObjects(Name: string; OfsX, OfsY: Word);
var
  F: file;
  I: Integer;
  PosX, PosY: Word;
  Count, FirstSlot, N: Byte;
begin
  {$I-}
  Assign(F, Name);
  Reset(F, 1);
  BlockRead(F, Count, 1);
  BlockRead(F, Actors[ExtraCount + 3], Count * 38);
  FirstSlot := SpriteCount;
  for I := 1 to Count do
  begin
    BlockRead(F, N, 1);
    AddSprite(N, 1, 1);
  end;
  for I := 1 to Count do
  begin
    BlockRead(F, PosX, 2);
    BlockRead(F, PosY, 2);
    with RenderEntries[FirstSlot + I - 1] do
    begin
      XBits := Word(OfsX + PosX);
      YBits := Word(OfsY + PosY);
    end;
  end;
  BlockRead(F, N, 1);
  Actors[ExtraCount + 3].Param12 := ExtraCount + 1;
  for I := 2 to Count do
    with Actors[ExtraCount + I + 2] do
    begin
      if Param0E and $FF > 0 then
      begin
        Inc(Param0E, MotionCount);
        if Param0E shr 8 > 0 then
        begin
          Inc(Param0E, Word(MotionCount) shl 8);
          if Param10 and $FF > 0 then
            Inc(Param10, MotionCount);
        end;
      end;
      OriginId := ExtraCount + 1;
    end;
  BlockRead(F, Motions[MotionCount + 1], N * 16);
  {$I+}
  if IOResult <> 0 then
    Shutdown(1);
  for I := MotionCount + 1 to MotionCount + N do
    with Motions[I] do
    begin
      Inc(PrimarySprite, FirstSlot);
      Inc(ReferenceSprite, FirstSlot);
    end;
  Inc(MotionCount, N);
  for I := 1 to Count do
    with Actors[ExtraCount + I + 2] do
    begin
      if I > 1 then
        Inc(CounterOrLink, FirstSlot);
      Inc(SpriteSlot, FirstSlot);
      if AnimationSelectorA > 0 then
        AnimationInit(Animation, Anims[AnimationSelectorA, 1],
                      Anims[AnimationSelectorA, 2], Animation.Delay, 1);
    end;
  Inc(ExtraCount, Count);
end;

{ 10C33 Reads the current level from LevelFile: size, counters, both
  packed map layers (unpacked through the background buffer), spawners, map
  points and tile replacements. The maps are allocated with 16 spare bytes
  and moved to offset 0 of the next paragraph. }
procedure ReadLevel;
var
  Size: Word;
  Dim: Word;
  Source: PByteWindow;
  Dest: PByteWindow;
begin
  if LevelOffsets[Level] <> FileSize(LevelFile) then
    Seek(LevelFile, LevelOffsets[Level]);
  if MapBAlloc <> nil then
  begin
    FreeMem(MapBAlloc, Word(LevelSize * 2 + 16));
    MapBAlloc := nil;
  end;
  if MapAAlloc <> nil then
  begin
    FreeMem(MapAAlloc, Word(LevelSize + 16));
    MapAAlloc := nil;
  end;
  {$I-}
  BlockRead(LevelFile, Dim, 2);
  MapWidth := Dim;
  BlockRead(LevelFile, Dim, 2);
  CamMaxY := Integer((Dim - 21) * 8);
  LevelSize := Word(Dim * MapWidth);
  BlockRead(LevelFile, CountedTile, 1);
  BlockRead(LevelFile, BonusRequired, 2);
  BlockRead(LevelFile, TargetPercent, 1);
  GetMem(MapA, Word(LevelSize + 16));
  MapAAlloc := MapA;
  if Ofs(MapA^) <> 0 then
    if Ofs(MapA^) < 16 then
      MapA := Ptr(Seg(MapA^) + 1, 0)
    else
      Halt(0);
  GetMem(MapB, Word(LevelSize * 2 + 16));
  MapBAlloc := MapB;
  if Ofs(MapB^) <> 0 then
    if Ofs(MapB^) < 16 then
      MapB := Ptr(Seg(MapB^) + 1, 0)
    else
      Halt(0);
  MapSeg := Seg(MapA^);
  BlockRead(LevelFile, Size, 2);
  BlockRead(LevelFile, Background^, Size);
  Source := Background;
  Dest := MapA;
  LegacyExpandUnsafe(Source, Size, Dest, LevelSize);
  BlockRead(LevelFile, Size, 2);
  BlockRead(LevelFile, Background^, Size);
  LevelSize := Word(LevelSize * 2);
  Dest := MapB;
  LegacyExpandUnsafe(Source, Size, Dest, LevelSize);
  LevelSize := LevelSize div 2;
  BlockRead(LevelFile, NextTag, 2);
  BlockRead(LevelFile, BuildingCells, 2);
  BlockRead(LevelFile, SpawnCount, 1);
  if SpawnCount > 0 then
    BlockRead(LevelFile, Spawns, SpawnCount * 30);
  BlockRead(LevelFile, PointCount, 1);
  if PointCount > 0 then
    BlockRead(LevelFile, MapPoints, PointCount * 7);
  BlockRead(LevelFile, ReplCount, 1);
  if ReplCount > 0 then
    BlockRead(LevelFile, Replacements, ReplCount * 14);
  {$I+}
  if IOResult <> 0 then
    Shutdown(1);
end;

{ 10FAA }
procedure LoadLevel;
var
  Unused: string;
begin
  ReadLevel;
end;

{ 10FBE INT 1Ch: plays the current sound. NoteIndex above 60000 is a falling
  sweep; otherwise it steps through Sounds^ (1-based, 30000 ends). }
procedure SoundTimerHandler; interrupt;
type
  TSoundTable = array[1..10922] of TSound6;
  PSoundTable = ^TSoundTable;
begin
  if SoundOn <> 0 then
  begin
    if NoteIndex > 60000 then
    begin
      TPSound(Word(NoteIndex - 59970));
      Dec(NoteIndex, 4);
      if NoteIndex <= 60000 then
      begin
        SoundOn := 0;
        TPNoSound;
      end;
    end
    else
    begin
      Inc(SoundTick);
      if SoundTick = SoundStep then
      begin
        Inc(NoteIndex);
        with PSoundTable(Sounds)^[NoteIndex] do
          if ToneParameter = 30000 then
          begin
            TPNoSound;
            SoundOn := 0;
            SoundTick := 0;
            SoundStep := 1;
          end
          else
          begin
            TPSound(ToneParameter);
            SoundGate := GateTicks;
            SoundStep := StepTicks;
            SoundTick := 0;
          end;
      end
      else if SoundTick = SoundGate then
        TPNoSound;
    end;
  end;
end;

{ 11091 INT 9: sets Keys[] to 1 on make codes and 0 on break codes of M Z X
  N C, the arrows and Ins, then chains to the previous handler. }
procedure KeyboardHandler; interrupt;
begin
  asm
    in al, $60
    mov ah, 1
    cmp al, $2C
    jne @NoZ
    mov byte ptr [Keys + 1], ah
  @NoZ:
    cmp al, $2D
    jne @NoX
    mov byte ptr [Keys + 2], ah
  @NoX:
    cmp al, $32
    jne @NoM
    mov byte ptr [Keys], ah
  @NoM:
    cmp al, $31
    jne @NoN
    mov byte ptr [Keys + 3], ah
  @NoN:
    cmp al, $2E
    jne @NoC
    mov byte ptr [Keys + 4], ah
  @NoC:
    cmp al, $4B
    jne @NoLeft
    mov byte ptr [Keys + 6], ah
  @NoLeft:
    cmp al, $4D
    jne @NoRight
    mov byte ptr [Keys + 7], ah
  @NoRight:
    cmp al, $52
    jne @NoIns
    mov byte ptr [Keys + 8], ah
  @NoIns:
    cmp al, $48
    jne @NoUp
    mov byte ptr [Keys + 5], ah
  @NoUp:
    cmp al, $50
    jne @NoDown
    mov byte ptr [Keys + 9], ah
  @NoDown:
    xor ah, ah
    cmp al, $AC
    jne @UpZ
    mov byte ptr [Keys + 1], ah
  @UpZ:
    cmp al, $AD
    jne @UpX
    mov byte ptr [Keys + 2], ah
  @UpX:
    cmp al, $B2
    jne @UpM
    mov byte ptr [Keys], ah
  @UpM:
    cmp al, $B1
    jne @UpN
    mov byte ptr [Keys + 3], ah
  @UpN:
    cmp al, $AE
    jne @UpC
    mov byte ptr [Keys + 4], ah
  @UpC:
    cmp al, $CB
    jne @UpLeft
    mov byte ptr [Keys + 6], ah
  @UpLeft:
    cmp al, $CD
    jne @UpRight
    mov byte ptr [Keys + 7], ah
  @UpRight:
    cmp al, $D2
    jne @UpIns
    mov byte ptr [Keys + 8], ah
  @UpIns:
    cmp al, $C8
    jne @UpUp
    mov byte ptr [Keys + 5], ah
  @UpUp:
    cmp al, $D0
    jne @UpDown
    mov byte ptr [Keys + 9], ah
  @UpDown:
    pushf
    call dword ptr [OldInt9]
  end;
end;
