{ LZTEXT.INC -- original linear 0x11156..0x12078: rolling score digits, the
  bitmap-font text writer, the sound request, the high-score table and the
  end-of-level / end-of-game screens. }

{ 11156 copy the 8x8 tile at DS:TmpB to video memory at A000:TmpA }
procedure PutTile8; assembler;
asm
  mov di, TmpA
  mov ax, $A000
  push ax
  pop es
  mov cx, 8
  mov si, TmpB
@row:
  push cx
  mov cx, 4
  rep movsw
  add di, $138
  pop cx
  loop @row
end;

{ 11174 the counted (bonus) tile and tile 117 below it in the goal panel }
procedure DrawGoalIcons(PanelOfs: Word);
begin
  TmpA := PanelOfs + 4 * 320 + 1;
  TmpB := CountedTile shl 6 + TileBase;
  PutTile8;
  Inc(TmpA, 16 * 320);
  TmpB := TileBase + 117 * 64;
  PutTile8;
end;

{ 111B0 target digit tiles (up to eight, least significant first) of the
  score; DisplayState 1 = rolling }
procedure SetScoreTargets(var Display: TScoreDisplay45);
var
  Value: LongInt;
  I: Word;
  Digit: Byte;
begin
  Value := Display.Score;
  I := 0;
  repeat
    Digit := Value mod 10;
    Display.TargetGlyphs[I] := Digit shl 6 + DigitBase;
    Value := Value div 10;
    Inc(I);
  until (Value = 0) or (I = 8);
  Display.DisplayState := 1;
end;

{ 11239 draw the nine current digit tiles of the display at byte offset
  DisplayOfs from ScoreDisp1 (0 or ScoreGap) to A000:TmpB, nine pixels apart,
  without leading zeros (the last digit is always drawn) }
procedure DrawScoreDigits(DisplayOfs: Word);
begin
  asm
    mov ax, $A000
    push ax
    pop es
    mov ax, TmpB
    mov TmpA, ax
    mov bx, $10
    add bx, DisplayOfs
    mov cx, 9
    xor dl, dl
  @digit:
    push cx
    mov ax, word ptr [bx + ScoreDisp1 + 4]     { CurrentGlyphs }
    cmp dl, 1
    je @draw
    cmp ax, DigitBase
    je @skip
  @draw:
    mov TmpB, ax
    call PutTile8
    mov dl, 1
  @skip:
    add TmpA, 9
    sub bx, 2
    pop cx
    cmp cx, 2
    jne @next
    mov dl, 1
  @next:
    loop @digit
  end;
end;

{ 11287 roll each current digit tile one or two pixel rows towards its
  target; DisplayState 2 = all digits arrived }
procedure RollScoreDigits(DisplayOfs: Word);
begin
  asm
    mov bx, $10
    add bx, DisplayOfs
    xor dl, dl
    mov cx, 9
  @digit:
    mov ax, word ptr [bx + ScoreDisp1 + 4]     { CurrentGlyphs }
    cmp ax, word ptr [bx + ScoreDisp1 + 24]    { TargetGlyphs }
    je @next
    add ax, 8
    test cx, 1
    je @even
    add ax, 8
  @even:
    inc dl
    sub ax, $280
    cmp ax, DigitBase
    jae @store
    add ax, $280
  @store:
    mov word ptr [bx + ScoreDisp1 + 4], ax
  @next:
    sub bx, 2
    loop @digit
    cmp dl, 0
    jne @done
    mov al, 2
    mov bx, DisplayOfs
    mov byte ptr [bx + ScoreDisp1 + 44], al    { DisplayState }
  @done:
  end;
end;

{ 112DB draw sprite Ch at screen (X, Y): pixel 0 = ByteTemp (0 transparent),
  pixel 1 = ByteTemp2, other pixels as stored }
procedure DrawGlyph(X, Y: Word; Ch: Byte);
var
  Bitmap: Word;
  H: Byte;
  W: Byte;
begin
  with SpriteDefs[Ch] do
  begin
    Bitmap := BitmapOffset;
    W := Width;
    H := Height;
  end;
  asm
    mov ax, $A000
    push ax
    pop es
    mov bx, 320
    mov ax, Y
    mov di, X
    mul bx
    add di, ax
    mov cl, H
    mov si, Bitmap
    xor ch, ch
    mov ah, ByteTemp
    mov bl, ByteTemp2
    mov bh, W
    push ds
    mov ds, SpriteSeg
  @row:
    push cx
    push di
    mov cl, bh
    xor ch, ch
  @pixel:
    lodsb
    cmp al, 0
    jne @notzero
    mov al, ah
    cmp al, 0
    jne @put
    inc di
    jmp @next
  @notzero:
    cmp al, 1
    jne @put
    mov al, bl
  @put:
    stosb
  @next:
    loop @pixel
    pop di
    pop cx
    add di, 320
    loop @row
    pop ds
  end;
end;

{ 1136E draw S in the sprite font: 'a'..'z' are sprites FirstGlyph..+25,
  '0'..'?' are FirstGlyph+26..+41, blanks are skipped; an optional shadow
  copy is drawn first at (ShadowX, ShadowY) relative }
procedure DrawString(X, Y, ShadowX, ShadowY: Integer; S: string;
  Colour, ShadowColour, Spacing, FirstGlyph: Byte);
var
  I: Integer;
  CharX: Integer;
  Glyph: Integer;
  Spare: Integer;   { [bp-108h] of the original frame, never used }
  C: Char;
begin
  ByteTemp := 0;
  for I := 1 to Length(S) do
  begin
    C := S[I];
    CharX := (I - 1) * Spacing + X;
    if C <> ' ' then
    begin
      Glyph := Ord(C) - $61 + FirstGlyph;
      if (C >= '0') and (C <= '?') then
      begin
        Glyph := Ord(C) - $30;
        Inc(Glyph, FirstGlyph + 26);
      end;
      if ShadowColour <> 0 then
      begin
        ByteTemp2 := ShadowColour;
        DrawGlyph(CharX + ShadowX, Y + ShadowY, Glyph);
      end;
      ByteTemp2 := Colour;
      DrawGlyph(CharX, Y, Glyph);
    end;
  end;
end;

{ 1146A typewriter text: a trail Part of TrailLen = ColourHigh - ColourLow + 1
  characters slides over S (padded with TrailLen blanks on both sides); its
  characters get the colours ColourHigh down to ColourLow, the newest one a
  shadow. Pause ms per step; a key ends the pauses (TypingKey 2, Esc 1).
  Centre <> 0 centres the text on the screen. Nothing happens if
  TrailLen > 6. }
procedure TypeText(X: Integer; Y: Word; S: string;
  ColourLow, ColourHigh, ShadowColour, Spacing, FirstGlyph: Byte;
  Pause: Word; Centre: Byte);
var
  Part: string[6];
  Left: Integer;
  I: Integer;
  J: Integer;
  TrailLen: Integer;
  Shadow: Byte;
begin
  TypingKey := 0;
  if Centre <> 0 then
    X := 160 - Word(Length(S)) * Word(Spacing) div 2;
  Part := '      ';
  TrailLen := ColourHigh - ColourLow + 1;
  if TrailLen < 7 then
  begin
    Left := X - (TrailLen + 1) * Spacing;
    S := Copy(Part, 1, TrailLen) + S + Copy(Part, 1, TrailLen);
    for I := 1 to Length(S) - TrailLen do
    begin
      Part := Copy(S, I, TrailLen);
      for J := 1 to TrailLen do
      begin
        if J = TrailLen then
          Shadow := ShadowColour
        else
          Shadow := 0;
        DrawString(Spacing * J + Left, Y, -1, -1, Part[J], ColourHigh - J + 1,
          Shadow, Spacing, FirstGlyph);
      end;
      Delay(Pause);
      if KeyPressed then
      begin
        Pause := 0;
        LastKey := ReadKey;
        TypingKey := 2;
        if LastKey = #27 then
          TypingKey := 1;
      end;
      Inc(Left, Spacing);
    end;
  end;
end;

{ 1165A start sound TmpB with priority SoundRequest unless a sound of a
  higher priority is playing }
procedure RequestSound; assembler;
asm
  mov al, SoundOn
  cmp al, 0
  je @start
  mov al, SoundPriority
  dec al
  cmp al, SoundRequest
  jge @done
@start:
  mov al, SoundRequest
  mov SoundPriority, al
  mov ax, TmpB
  mov NoteIndex, ax
  mov SoundOn, 1
@done:
end;

{ 116CA write RECS.DAT: a count byte (7), then score (4) and name (9) of
  each entry; on failure offer Enter = retry, Esc = give up }
procedure SaveHighScores;
label
  Retry;
var
  F: file;
  I: Byte;
  Count: Byte;
begin
Retry:
  {$I-}
  Assign(F, 'recs.dat');
  Rewrite(F, 1);
  Count := 7;
  BlockWrite(F, Count, 1);
  for I := 1 to 7 do
    with HighScores[I] do
    begin
      BlockWrite(F, Score, 4);
      BlockWrite(F, Name, 9);
    end;
  Close(F);
  {$I+}
  if IOResult <> 0 then
  begin
    FillBox(0, 0, 320, 200, 0);
    DrawString(10, 89, 1, 1, 'impossibile creare', 55, 13, 9, 27);
    DrawString(10, 100, 1, 1, 'il file dei records', 55, 13, 9, 27);
    DrawString(10, 111, 1, 1, 'enter;riprova   esc;annulla', 55, 13, 9, 27);
    repeat
      LastKey := ReadKey;
    until (LastKey = #27) or (LastKey = #13);
    if LastKey = #13 then
      goto Retry;
  end;
end;

{ 11B14 end of the game (Reason 1 = game over, 2 = all levels done): final
  scores, then name entry for every score that reaches the table }
procedure EndGame(Reason: Byte);
var
  Y: Word;
  I: Word;
  T: string;
  Scores: array[1..2] of LongInt;

  { 11845 insert NewScore into the high-score table with a typed name }
  procedure EnterName(NewScore: LongInt; Who: Word);
  var
    Rank: Byte;
    Cursor: Byte;
    RankStr: string[2];
    NewName: string[8];
  begin
    DrawWaveBands;
    TmpB := 120;
    SoundRequest := 11;
    RequestSound;
    Rank := 7;
    while (Rank >= 1) and (HighScores[Rank].Score < NewScore) do
      Dec(Rank);
    Inc(Rank);
    Str(Rank, RankStr);
    TypeText(60, 10, 'giocatore ' + PlayerDigits[Who], 27, 31, 25, 11, 27,
      TypeDelay, 1);
    TypeText(60, 21, 'hai ottenuto il ' + RankStr + ' posto', 27, 31, 25, 11,
      27, TypeDelay, 1);
    TypeText(60, 32, 'nei records >>>', 27, 31, 25, 11, 27, TypeDelay, 1);
    TypeText(80, 43, 'inserisci il tuo nome;', 27, 31, 25, 11, 27, TypeDelay, 1);
    DrawBevelBox(122, 58, 76, 13, 7, 8);
    NewName := '::::::::';
    Cursor := 1;
    repeat
      DrawString(124, 60, 1, 1, NewName, 77, 55, 9, 27);
      LastKey := ReadKey;
      LastKey := UpCase(LastKey);
      if (((LastKey >= 'A') and (LastKey <= 'Z')) or (LastKey = ' ')) and
         (Cursor < 9) then
      begin
        LastKey := Chr(Ord(LastKey) or $20);
        NewName[Cursor] := LastKey;
        FillBox(Cursor * 9 + 115, 60, 10, 10, 8);
        Inc(Cursor);
        TmpB := 0;
        SoundRequest := Cursor;
        RequestSound;
      end
      else
        if (LastKey = #8) and (Cursor > 1) then
        begin
          NewName[Cursor - 1] := ':';
          Dec(Cursor);
          FillBox(Cursor * 9 + 115, 60, 10, 10, 8);
        end;
    until LastKey = #13;
    TmpB := 8;
    SoundRequest := 11;
    RequestSound;
    if Rank < 7 then
      for GI := 7 downto Rank + 1 do
        HighScores[GI] := HighScores[GI - 1];
    { Rank is 8 when NewScore only ties HighScores[7]: unchecked index }
    with HighScores[Rank] do
    begin
      Name := NewName;
      Score := NewScore;
    end;
    SaveHighScores;
  end;

begin
  DrawWaveBands;
  Close(LevelFile);
  if SpriteSet <> 1 then
  begin
    LoadSprites('fonts.spr');
    if IOResult <> 0 then
      Shutdown(1);
    SpriteSet := 1;
  end;
  Y := 77;
  Flag := 0;
  case Reason of
    1:
      TypeText(60, 77, 'game over', 27, 31, 25, 11, 1, TypeDelay, 1);
    2:
      begin
        TypeText(60, 55, 'eccellente>>>', 27, 31, 25, 11, 27, TypeDelay, 1);
        TypeText(60, 66, 'hai completato il gioco', 27, 31, 25, 11, 1,
          TypeDelay, 1);
        Flag := 1;
      end;
  end;
  Inc(Y, 20);
  TypeText(60, Y, Texts[Language, 10], 240, 244, 25, 11, 1, TypeDelay, 1);
  Inc(Y, 24);
  for I := 1 to 2 do
  begin
    if I = 1 then
    begin
      ScorePtr := @ScoreDisp1;
      LastKey := '1';
    end
    else
    begin
      ScorePtr := @ScoreDisp2;
      LastKey := '2';
    end;
    Scores[I] := TScoreDisplay45(ScorePtr^).Score;
    if Scores[I] > 0 then
    begin
      Str(TScoreDisplay45(ScorePtr^).Score, T);
      T := Texts[Language, 11] + LastKey + '; ' + T;
      TypeText(60, Y, T, 240, 244, 25, 9, 27, TypeDelay, 1);
      Inc(Y, 11);
    end;
  end;
  LastKey := ReadKey;
  for I := 1 to 2 do
    if Scores[I] >= HighScores[7].Score then
      EnterName(Scores[I], I);
end;

{ 11D61 level completed: destruction bonus (10 per destroyed cell) and bomb
  bonus (100/500/2000 per bomb of kinds 2/3/4) are added to the scores of the
  active players, then the next level is selected }
procedure LevelCompleted;
var
  Y: Word;
  I: Integer;
  GapOfs: Word;
  Spare: Word;      { [bp-8] of the original frame, never used }
  T: string;
  N: string;
  Bonus: LongInt;
  DestrBonus: LongInt;
  Disp: ^TScoreDisplay45;
  HudOfs: Word;
begin
  if SpriteSet <> 1 then
  begin
    LoadSprites('fonts.spr');
    if IOResult <> 0 then
      Shutdown(1);
    SpriteSet := 1;
  end;
  Delay(500);
  TmpB := 61;
  SoundRequest := 10;
  RequestSound;
  while KeyPressed do
    LastKey := ReadKey;
  Y := 60;
  SetRGB(255, 31, 31, 31);
  { 16-bit product, zero-extended }
  DestrBonus := Word(Destroyed * 10);
  TypeText(60, Y, Texts[Language, 13], 27, 31, 25, 11, 1, TypeDelay, 1);
  Inc(Y, 21);
  Str(DestrBonus, N);
  TypeText(60, Y, Texts[Language, 12] + N, 240, 244, 241, 9, 27, TypeDelay, 1);
  Inc(Y, 18);
  TypeText(60, Y, 'bomba bonus', 240, 244, 25, 9, 27, TypeDelay, 1);
  Inc(Y, 21);
  for I := 1 to 2 do
    if PlayerState[I] = 1 then
    begin
      if I = 1 then
      begin
        Disp := @ScoreDisp1;
        GapOfs := 0;
      end
      else
      begin
        Disp := @ScoreDisp2;
        GapOfs := ScoreGap;
      end;
      Str(I, T);
      T := Texts[Language, 11] + ' ' + T + '   ';
      { 16-bit Integer sum, sign-extended }
      Bonus := Integer(Bombs[I, 2] * 100 + Bombs[I, 3] * 500 +
        Bombs[I, 4] * 2000);
      Str(Bonus, N);
      T := T + N;
      TypeText(60, Y, T, 27, 31, 13, 9, 27, TypeDelay, 1);
      Inc(Y, 11);
      HudOfs := HudX[I] + $DC00;
      Inc(Disp^.Score, Bonus + DestrBonus);
      SetScoreTargets(Disp^);
      while Disp^.DisplayState < 2 do
      begin
        RollScoreDigits(GapOfs);
        TmpB := HudOfs;
        DrawScoreDigits(GapOfs);
        Delay(15);
        if TPRandom(4) > 2 then
        begin
          TmpB := 33;
          RequestSound;
        end;
      end;
      Delay(200);
    end;
  LastKey := ReadKey;
  Inc(Level);
  for I := 1 to 2 do
    if PlayerState[I] = 2 then
      PlayerState[I] := 1;
end;
