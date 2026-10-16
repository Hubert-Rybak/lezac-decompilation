{ LZMENU.INC -- main menu, start-up, game and level initialisation, actor
  spawning/removal and HUD counters: original linear 0x12079..0x1346A.
  Names are descriptive choices, not recovered identifiers. }

{ 12361 }
{ Main menu: '1'/'2' players, 'i' instructions, 'z' more pages,
  'r' high scores, 'l' language, Esc quits (Players = 0) }
procedure MainMenu;
label
  Restart;
var
  Done: Boolean;

  { 12079 }
  { Menu key click }
  procedure MenuClick;
  begin
    TmpB := $24;
    SoundRequest := 2;
    RequestSound;
  end;

  { 120AC }
  { High-score table }
  procedure ShowHighScores;
  var
    I: Integer;
    Y: Word;
    NumStr: string;
  begin
    DrawWaveBands;
    Y := 72;
    TypeText(9, 10, 'punteggi migliori', 6, 10, 6, 9, 27, TypeDelay, 1);
    for I := 1 to 7 do
      with HighScores[I] do
      begin
        Str(Score, NumStr);
        TypeText(9, Y, Name + '    ' + NumStr, 6, 10, 6, 9, 27, TypeDelay, 1);
        Y := Y + 11;
      end;
    LastKey := TPReadKey;
  end;

  { 1216C }
  { Next '\'-separated line of a text page; CharPos is left after the
    separator. S[CharPos] is tested before the length check. }
  function NextLine(var S: string; var CharPos: Integer;
                    var LastLine: Boolean): string;
  var
    Line: string;
  begin
    Line := '';
    while (S[CharPos] <> '\') and (Length(S) >= CharPos) do
    begin
      Line := Line + S[CharPos];
      Inc(CharPos);
    end;
    NextLine := Line;
    Inc(CharPos);
    if Length(S) < CharPos then
      LastLine := True
    else
      LastLine := False;
  end;

  { 12226 }
  { Types the text pages First..Last; a line '*' waits for a key and
    starts a new screen. Paged: fade first, Esc (TypingKey 1) aborts, wait
    for a key at the end. Fast: a key (TypingKey 2) stops the typing delay. }
  procedure ShowPages(First, Last: Byte; Y: Word; Paged, Fast: Boolean);
  var
    Page: string;
    Line: string;
    Unused: Word;       { bp-202h, never referenced }
    CharPos: Integer;
    LineY: Word;
    Speed: Byte;
    LastLine: Boolean;
  begin
    Speed := Lo(TypeDelay);
    if Paged then
      DrawWaveBands;
    LineY := Y;
    GI := First;
    while GI <= Last do
    begin
      Page := Texts[Language, GI];
      CharPos := 1;
      repeat
        Line := NextLine(Page, CharPos, LastLine);
        if Line = '*' then
        begin
          LastKey := TPReadKey;
          DrawWaveBands;
          LineY := 20;
        end
        else
        begin
          TypeText(9, LineY, Line, 6, 10, 6, 9, 27, Speed, 1);
          LineY := LineY + 10;
          if (TypingKey = 2) and Fast then
            Speed := 0;
          if (TypingKey = 1) and Paged then
            Exit;
        end;
      until LastLine;
      Inc(GI);
    end;
    if Paged then
      LastKey := TPReadKey;
  end;

begin
  if SpriteSet <> 1 then
  begin
    LoadSprites('fonts.spr');
    if IOResult <> 0 then
      Shutdown(1);
    SpriteSet := 1;
  end;
  if Flag = 1 then
    ShowPages(1, 3, 20, True, False);
Restart:
  FadeInPicture('sfonlef.zbg');
  ShowPages(8, 8, 77, False, True);
  Done := False;
  while TPKeyPressed do
    LastKey := TPReadKey;
  repeat
    LastKey := TPReadKey;
    if LastKey = '1' then
    begin
      MenuClick;
      Players := 1;
      Done := True;
    end;
    if LastKey = '2' then
    begin
      MenuClick;
      Players := 2;
      Done := True;
    end;
    if LastKey = 'i' then
    begin
      MenuClick;
      ShowPages(1, 3, 20, True, False);
      goto Restart;
    end;
    if LastKey = 'z' then
    begin
      MenuClick;
      ShowPages(4, 7, 20, True, False);
      goto Restart;
    end;
    if LastKey = 'r' then
    begin
      MenuClick;
      ShowHighScores;
      goto Restart;
    end;
    if LastKey = 'l' then
    begin
      MenuClick;
      Language := Language xor 1;
      goto Restart;
    end;
    if LastKey = #27 then
    begin
      Done := True;
      Players := 0;
    end;
  until Done;
end;

{ 126E8 }
{ One-time start-up: delays, keyboard rate, sine table, interrupt
  handlers, memory, text checksum, tiles, sounds, graphics, high scores }
procedure InitGame;
const
  { 6.28 = CX:SI:DI 5C83h:C28Fh:48F5h, 128 = 0088h:0:0 (1272C, 12737) }
  RealSinScale: TReal = (W0: $5C83; W1: $C28F; W2: $48F5);
  RealSinSteps: TReal = (W0: $0088; W1: $0000; W2: $0000);
var
  Check: Word;

  { 1247F }
  { Measures the hundredths of a second that TPDelay(1000) takes and
    derives the delays from it }
  procedure CalibrateDelays;
  const
    { 8000, 1600 and 200 as AX:BX:DX (12514, 12538, 1255C) }
    Real8000: TReal = (W0: $008D; W1: $0000; W2: $7A00);
    Real1600: TReal = (W0: $008B; W1: $0000; W2: $4800);
    Real200: TReal = (W0: $0088; W1: $0000; W2: $4800);
  var
    Ticks: Integer;
  begin
    TPDelay(500);
    repeat
      asm
        mov ax, $2C00
        int $21
        mov Second1, dh
        mov Hundredth1, dl
        mov Minute1, cl
      end ['AX', 'CX', 'DX'];
      TPDelay(1000);
      asm
        mov ax, $2C00
        int $21
        mov Second2, dh
        mov Hundredth2, dl
        mov Minute2, cl
      end ['AX', 'CX', 'DX'];
      Ticks := Integer((Minute2 - Minute1) * 6000 + (Second2 - Second1) * 100 +
        Hundredth2 - Hundredth1);
    until (Ticks > 0) and (Ticks < 300);
    TypeDelay := Word(RRound(RDiv(Real8000, RFromLong(Ticks))));
    StartDelay := Word(RRound(RDiv(Real1600, RFromLong(Ticks))));
    DelayStep := Word(RRound(RDiv(Real200, RFromLong(Ticks))));
    if DelayStep < 2 then
      DelayStep := 2;
  end;

  { 12597 }
  { Reads RECS.DAT, or creates the default table when it is missing }
  procedure LoadHighScores;
  var
    I: Integer;
    SR: SearchRec;
    F: file;
    Count: Byte;
    Unused: array[1..14] of Byte;   { frame bytes bp-0BEh..bp-0B0h, never
                                      referenced }
  begin
    SavedBreak := CheckBreak;
    CheckBreak := False;
    TPRandomize;
    FindFirst('recs.dat', Archive, SR);
    Flag := 0;
    if DosError <> 0 then
    begin
      for I := 1 to 7 do
        with HighScores[I] do
        begin
          Name := 'nessuno ';
          Score := 10000;
        end;
      SaveHighScores;
    end
    else
    begin
      {$I-}
      Assign(F, 'recs.dat');
      Reset(F, 1);
      BlockRead(F, Count, 1);
      for I := 1 to Count do
        with HighScores[I] do
        begin
          BlockRead(F, Score, 4);
          BlockRead(F, Name, 9);
        end;
      Close(F);
      {$I+}
      if IOResult <> 0 then
        Shutdown(1);
    end;
  end;

begin
  CalibrateDelays;
  SpriteCount := 0;
  MotionCount := 0;
  { keyboard typematic rate/delay }
  Port[$60] := $F3;
  TPDelay(200);
  Port[$60] := $67;
  for GI := 0 to 127 do
    SinTable[GI] := RSin(RDiv(RMul(RFromLong(GI), RealSinScale), RealSinSteps));
  Language := 0;
  SoundOn := 0;
  SpriteSet := 0;
  ScoreGap := Word(Ofs(ScoreDisp2) - Ofs(ScoreDisp1));
  GetIntVec(9, OldInt9);
  SetIntVec(9, @KeyboardHandler);
  GetIntVec($1C, OldInt1C);
  SetIntVec($1C, @SoundTimerHandler);
  { the first test compares with the size of the pointer (4), not 792 }
  if MaxAvail < SizeOf(Sounds) then
    Shutdown(2)
  else
    GetMem(Sounds, 792);
  if MaxAvail < 57000 then
    Shutdown(2)
  else
    GetMem(Screen, 57000);
  BackSeg := Seg(Screen^);
  BackOfs := Ofs(Screen^);
  TileBase := Ofs(Tiles);
  if MaxAvail < 21300 then
    Shutdown(2)
  else
    GetMem(SpriteData, 21300);
  MapWidth := 40;
  ViewHeight := 160;
  RenderTableOfs := Ofs(RenderEntries);
  SpriteSeg := Seg(SpriteData^);
  if MaxAvail < 60000 then
    Shutdown(2)
  else
    GetMem(Background, 60000);
  if MaxAvail < 34600 then
    Shutdown(2);
  Check := 0;
  AlternatingChecksum(Texts, Check, $1A00);
  if Check <> $FD37 then
  begin
    SetIntVec(9, OldInt9);
    SetIntVec($1C, OldInt1C);
    Halt;
  end;
  LoadTiles('caro.car');
  if IOResult <> 0 then
    Shutdown(1);
  LoadSounds;
  SoundTick := 0;
  SoundStep := 1;
  ViewTilesSaved := 40;
  InitGraph;
  DigitBase := TileBase + $1E40;        { tile 121 }
  if SpriteSet <> 1 then
  begin
    LoadSprites('fonts.spr');
    if IOResult <> 0 then
      Shutdown(1);
    SpriteSet := 1;
  end;
  DebrisBase := Ofs(Debris);
  GroupBase := Ofs(Groups);
  LoadHighScores;
  CalibrateDelays;
  FrameDelay := StartDelay;
end;

{ 12959 }
{ Frame(s) around the view(s) and their screen offsets }
procedure DrawPlayfieldFrame;
begin
  case Players of
    1:
      begin
        DrawBevelBox(Word(160 - ViewTilesSaved * 4), 0, Word(ViewTilesSaved * 8),
          160, 15, 28);
        ViewOfs[1] := Word(164 - ViewTilesSaved * 4 + 4 * 320);
        ViewOfs[2] := ViewOfs[1];
      end;
    2:
      begin
        DrawBevelBox(0, 0, 160, 160, 15, 28);
        DrawBevelBox(160, 0, 160, 160, 4, 12);
        ViewOfs[1] := 4 * 320 + 4;
        ViewOfs[2] := 4 * 320 + 164;
      end;
  end;
end;

{ 129DB }
{ Three life icons of a player (tile 115, empty slots tile 0) }
procedure DrawLives(Player: Word);
var
  I: Integer;
  X: Integer;
  Count: Byte;
begin
  Count := Lives[Player];
  X := Integer(HudX[Player]);
  TmpB := TileBase + $1CC0;             { tile 115 }
  TmpA := Word(X + 192 * 320);
  for I := 1 to 3 do
  begin
    if Count < I then
      TmpB := TileBase;
    PutTile8;
    TmpA := TmpA + 9;
  end;
end;

{ 12ADC }
{ Level start: state reset, level title, sprites, HUD, both players }
procedure StartLevel;
var
  LevelStr: string;

  { 12A41 }
  { HUD boxes of one player at X }
  procedure DrawHudBox(X: Byte);
  begin
    DrawBevelBox(X + 0, 172, 88, 17, 3, 239);
    FillBox(X + 0, 164, 102, 3, 28);
    DrawBevelBox(X + 119, 161, 20, 20, 27, 25);
    FillBox(X + 119, 181, 20, 9, 239);
  end;

begin
  { flush the DOS keyboard buffer }
  asm
    mov ax, $0C06
    mov dl, $FF
    int $21
  end ['AX', 'DX'];
  SelBomb[1] := 1;
  SelBomb[2] := 1;
  LoadLevel;
  ShownPercent := 200;
  BonusShown := 20000;
  CanContinue := 1;
  Str(Level, LevelStr);
  MapBSeg := Seg(MapB^);
  BonusDone := 0;
  DestructionDone := 0;
  FineX := 0;
  FineY := 0;
  MapViewOfs := Ofs(MapA^);
  Keys[1] := 0;
  Keys[3] := 0;
  Keys[2] := 0;
  Keys[4] := 0;
  ShakeOffset := 0;
  Keys[7] := 0;
  Keys[8] := 0;
  Keys[9] := 0;
  Keys[6] := 0;
  Keys[5] := 0;
  Keys[10] := 0;
  HudX[2] := 180;
  HudX[1] := 0;
  Shaking := 0;
  ShakeTimer := 0;
  BombSwitch[1] := 0;
  BombSwitch[2] := 0;
  ExtraCount := 0;
  DebrisLast := 199;
  GroupLast := 0;
  ProjLast := 0;
  KindTenCount := 0;
  Destroyed := 0;
  BonusRain := 0;
  BonusCollected := 0;
  Percent := 0;
  FlashCount := 0;
  MotionCount := 0;
  RowBytes := MapWidth * 2;
  if Players = 2 then
    SetViewWidth(20)
  else
    SetViewWidth(ViewTilesSaved);
  LoadPalette('bompal.pal');
  DrawWaveBands;
  TmpB := $78;
  SoundRequest := 11;
  RequestSound;
  if SpriteSet <> 1 then
  begin
    LoadSprites('fonts.spr');
    if IOResult <> 0 then
      Shutdown(1);
    SpriteSet := 1;
  end;
  TypeText(60, 94, Texts[Language, 9] + LevelStr, 27, 31, 1, 11, 27,
    TypeDelay, 1);
  LastKey := TPReadKey;
  FillBox(0, 0, 320, 200, 0);
  if SpriteSet <> 2 then
  begin
    if Level <> 7 then
      LoadSprites('bomomimk.spr')
    else
      LoadSprites('prova.spr');
    if IOResult <> 0 then
      Shutdown(1);
    SpriteSet := 2;
  end;
  for GI := 1 to 2 do
  begin
    if PlayerState[GI] = 1 then
      DrawHudBox(Lo(HudX[GI]));
    ShownEnergy[GI] := $FF;
  end;
  DrawPlayfieldFrame;
  DrawBevelBox(141, 160, 37, 39, 3, 224);
  FillBox(143, 165, 10, 10, 245);
  FillBox(143, 181, 10, 10, 246);
  DrawGoalIcons(51983);               { 162 * 320 + 143 }
  if PlayerState[1] = 1 then
  begin
    TmpB := HudX[1] + $DC00;            { + 176 * 320 }
    DrawScoreDigits(0);
  end;
  if PlayerState[2] = 1 then
  begin
    TmpB := HudX[2] + $DC00;
    DrawScoreDigits(ScoreGap);
  end;
  SpriteCount := 1;
  RenderEntries[0].Width := SpriteDefs[1].Width;
  RenderEntries[0].Height := SpriteDefs[1].Height;
  RenderEntries[0].BitmapOffset := SpriteDefs[1].BitmapOffset;
  CamX := 0;
  CamY := 0;
  Inc(SpriteCount);
  RenderEntries[1].Width := SpriteDefs[1].Width;
  RenderEntries[1].Height := SpriteDefs[1].Height;
  RenderEntries[1].BitmapOffset := SpriteDefs[1].BitmapOffset;
  with Actors[1] do
  begin
    SpriteSlot := 0;
    VelocityX := 0;
    VelocityY := 0;
    FractionX := 0;
    FractionY := 0;
    Behavior := 0;
    Kind := 0;
    OriginYAdjustment := 0;
    Param0E := 0;
    AnimationInit(Animation, 2, 9, 1, 1);
  end;
  with Actors[2] do
  begin
    SpriteSlot := 1;
    VelocityX := 0;
    VelocityY := 0;
    FractionX := 0;
    FractionY := 0;
    Behavior := 1;
    Kind := 0;
    OriginYAdjustment := 0;
    Param0E := 0;
    AnimationInit(Animation, 21, 28, 1, 1);
  end;
  if PlayerState[1] = 1 then
    PlaceAtStartPoint(Actors[1], 1)
  else
  begin
    RenderEntries[0].XBits := 10000;
    RenderEntries[0].YBits := 6000;
  end;
  if PlayerState[2] = 1 then
    PlaceAtStartPoint(Actors[2], 2)
  else
  begin
    RenderEntries[1].XBits := 10000;
    RenderEntries[1].YBits := 6000;
  end;
  BombIconDirty[1] := 2;
  BombIconDirty[2] := 2;
  for GI := 1 to 2 do
    if PlayerState[GI] = 1 then
      DrawLives(GI);
  if Level = 7 then
    LoadExtraObjects('gran.mst', 100, 100);
end;

{ 12EFD }
{ New game: level file, players, lives, bombs, score displays }
procedure NewGame;

  { 12E8D }
  { Fills a score display with blank digits }
  procedure ResetDisplay(var D: TScoreDisplay45);
  begin
    with D do
      for GI := 0 to 9 do
      begin
        CurrentGlyphs[GI] := DigitBase;
        TargetGlyphs[GI] := DigitBase;
      end;
    D.DisplayState := 2;
    D.Score := 0;
  end;

begin
  {$I-}
  Assign(LevelFile, 'livels.sch');
  Reset(LevelFile, 1);
  {$I+}
  if IOResult <> 0 then
    Shutdown(1);
  Actors[1].Energy := 100;
  Actors[2].Energy := 100;
  Unknown79C7 := 0;
  PlayerState[1] := 1;
  PlayerState[2] := 1;
  ShowBackground := 1;
  if Players = 1 then
    PlayerState[2] := 0;
  Level := 1;
  Lives[1] := 2;
  Lives[2] := 2;
  Bombs[1, 1] := 200;
  Bombs[1, 2] := 20;
  Bombs[1, 3] := 6;
  Bombs[1, 4] := 0;
  Bombs[2] := Bombs[1];
  ResetDisplay(ScoreDisp1);
  ResetDisplay(ScoreDisp2);
end;

{ 12F9F }
{ New extra actor in Actors[ExtraCount + 2] with a render entry;
  TmpA = 1 on success, 0 when all 30 slots are taken }
procedure SpawnActor(X, Y: Word; VX, VY: Integer; Sprite: Word;
                     NewKind, NewLink, NewBehavior: Byte);
begin
  if ExtraCount < 30 then
  begin
    Inc(ExtraCount);
    TmpA := 1;
    { 16-bit Abs: -32768 stays negative and is not clamped }
    if Integer(Abs(VX)) > 2047 then
      VX := Integer(Abs(VX)) div VX * 2047;
    if Integer(Abs(VY)) > 2047 then
      VY := Integer(Abs(VY)) div VY * 2047;
    with Actors[ExtraCount + 2] do
    begin
      FractionX := 0;
      FractionY := 0;
      SpriteSlot := SpriteCount;
      VelocityX := VX;
      VelocityY := VY;
      Kind := NewKind;
      CounterOrLink := NewLink;
      Behavior := NewBehavior;
      if Sprite <> 31 then
        OriginYAdjustment := Shortint(16 - SpriteDefs[Sprite].Height)
      else
        OriginYAdjustment := 0;
      AddSprite(Sprite, X, Y);
    end;
  end
  else
    TmpA := 0;
end;

{ 130A3 }
{ Player lost his energy: counts the bonus tiles left in the map
  (CanContinue) and starts the dying animation }
procedure PlayerDies(Player: Byte);
begin
  TmpB := $56;
  SoundRequest := 5;
  RequestSound;
  asm
    xor si, si
    mov bl, CountedTile
    mov cx, LevelSize
    xor dx, dx
    push ds
    mov ds, MapSeg
  @count:
    lodsb
    cmp al, bl
    jne @next
    inc dx
  @next:
    loop @count
    pop ds
    mov TmpB, dx
  end ['AX', 'BX', 'CX', 'DX', 'SI'];
  if Word(BonusCollected + TmpB) >= BonusRequired then
    CanContinue := 1
  else
    CanContinue := 0;
  with Actors[Player] do
  begin
    AnimationInit(Animation, Anims[10, 1], Anims[10, 2], 3, 1);
    Behavior := 2;
    Param10 := 60;
    Energy := 100;
  end;
end;

{ 1313D }
{ Draws the lowest Digits decimal digits of Value from right to left,
  starting at screen offset TmpA }
procedure DrawDigits(Value, Digits: Word);
var
  Unused: Word;         { bp-2, never referenced }
begin
  while Digits > 0 do
  begin
    TmpB := Word((Value mod 10) * 64 + DigitBase);
    PutTile8;
    TmpA := TmpA - 9;
    Value := Value div 10;
    Dec(Digits);
  end;
end;

{ 13184 }
{ HUD counters: bonus still required and destruction percent still
  missing; a flash marks each change and the completion }
procedure UpdateGoalCounters(Base: Word);
var
  Rest: Integer;
begin
  TmpA := Word(Base + 4 * 320 + 25);
  if BonusShown <> BonusCollected then
  begin
    BonusShown := BonusCollected;
    StartFlash(245, FlashWhite, FlashDark);
    Rest := Integer(BonusRequired);
    Rest := Integer(Rest - BonusCollected);
    if Rest <= 0 then
    begin
      Rest := 0;
      BonusDone := 1;
    end;
    DrawDigits(Word(Rest), 2);
  end;
  if ShownPercent <> Percent then
  begin
    StartFlash(246, FlashWhite, FlashDark);
    ShownPercent := Percent;
    TmpA := Word(Base + 20 * 320 + 25);
    Rest := TargetPercent;
    if Percent < Rest then
      Rest := Rest - Percent
    else
    begin
      Rest := 0;
      DestructionDone := 1;
    end;
    DrawDigits(Word(Rest), 2);
  end;
  if (BonusDone <> 0) and (DestructionDone <> 0) then
    StartFlash(224, FlashWhite, FlashDark);
end;

{ 1326E }
{ Selected bomb (sprite 57 + slot) and its count (at most 99) }
procedure DrawBombIcon(X, Y: Word; Player: Byte);
var
  Sel: Byte;
  Count: Byte;
  IconX: Word;
  IconY: Word;
begin
  Sel := SelBomb[Player];
  Count := Bombs[Player, Sel];
  if BombIconDirty[Player] > 1 then
  begin
    FillBox(X, Y, 16, 16, 18);
    ByteTemp := 18;
    with SpriteDefs[Sel + 58 - 1] do
    begin
      IconX := Word(16 - Width) shr 1 + X;
      IconY := Word(16 - Height) shr 1 + Y;
    end;
    DrawGlyph(IconX, IconY, Byte(Sel + 58 - 1));
  end;
  BombIconDirty[Player] := 0;
  if Count > 99 then
    Count := 99;
  TmpA := Word((Y + 19) * 320 + X + 8);
  DrawDigits(Count, 2);
end;

{ 13358 }
{ Removes extra actor Index (Actors[Index + 2]) and its render entry
  and renumbers the sprite slots above it }
procedure RemoveActor(Index: Integer);
var
  I: Integer;
  Slot: Byte;
begin
  if ExtraCount >= Index then
  begin
    Slot := Actors[Index + 2].SpriteSlot;
    RemoveSprite(Slot);
    for I := Index to ExtraCount - 1 do
      Move(Actors[I + 1 + 2], Actors[I + 2], SizeOf(TActor38));
    Dec(ExtraCount);
    { from I = 0, i.e. Actors[2] (player 2) onwards }
    for I := 0 to ExtraCount do
      with Actors[I + 2] do
        if SpriteSlot > Slot then
          Dec(SpriteSlot);
    if MotionCount > 0 then
      for I := 1 to MotionCount do
        with Motions[I] do
        begin
          if PrimarySprite > Slot then
            Dec(PrimarySprite);
          if ReferenceSprite > Slot then
            Dec(ReferenceSprite);
        end;
  end;
end;
