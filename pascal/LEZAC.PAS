program Lezac;
{ Larax & Zaco v1.0 shareware -- Pascal reconstruction of the main program of
  LEZAC.EXE (code segment 1000, REA 0x10000..0x182CF) from the original
  instructions. Procedures appear in their original binary order; each one is
  marked with its original linear address. Names are descriptive choices, not
  recovered identifiers.

  Units: LZGRAPH (graphics unit, original segment 18AC), Crt, Dos, LZRLE
  (decompressor unit, original segment 182D), as in the original uses order.
  LZTYPES holds the record layouts; LZTPRT reproduces the Turbo Pascal 6
  runtime behaviour that Free Pascal does not share (6-byte Real arithmetic,
  Random, the Sound/NoSound port sequence, the standard INT 16h keyboard
  reads of Crt.KeyPressed/ReadKey, the calibrated Crt.Delay loop, the heap
  manager behind GetMem/FreeMem/MaxAvail). }
{$IFDEF FPC}{$MODE TP}{$ASMMODE INTEL}{$GOTO ON}{$ENDIF}
{$R-}{$Q-}{$S-}{$I+}{$B-}
{$M 16384,0,655360}

uses LZGRAPH, Crt, Dos, LZRLE, LZTYPES, LZTPRT;

label
  MenuLoop, LevelStart, Finish;

{$I LZDATA.INC}
{$I LZSYS.INC}
{$I LZTEXT.INC}
{$I LZMENU.INC}
{$I LZWORLD.INC}
{$I LZDEBRIS.INC}
{$I LZACTOR.INC}

{ Declarations of the main block. }
const
  { 100.0, loaded as CX:SI:DI = 0087h:0000h:4800h at 17985 }
  MainHundred: TReal = (W0: $0087; W1: $0000; W2: $4800);

var
  { Byte views [slot, component] of FlashNow/FlashGoal (no storage of their
    own): the flash fader indexes the colour components with ByteTemp. }
  FlashNowRGB: array[1..3, 1..3] of Byte absolute FlashNow;
  FlashGoalRGB: array[1..3, 1..3] of Byte absolute FlashGoal;
  { Stands for the word the original pushes at 178C7 (row part of a window
    dot position) so that the two Random calls keep their order. }
  MainTmp: Word;

{ 17783 main program: start-up, menu, level start and the frame loop }
begin
  MapBAlloc := nil;
  MapAAlloc := nil;
  TmpB := 0;
  InitGame;
  LastKey := '0';
MenuLoop:
  MainMenu;
  if Players = 0 then
    goto Finish;
  NewGame;
  FrameCount := 0;
  DirectVideo := False;
LevelStart:
  StartLevel;
  MakeBackground;
  BackgroundPitch := ViewWidth;
  BackgroundSkip := BackgroundPitch - ViewWidth;
  { Skyline in the background picture: ten blocks of colour $B0 reaching
    down to row 160 (the first one a fixed ground strip), the other nine with
    twenty window dots of colour $16. The time-stamp bytes serve as scratch. }
  for ByteTemp := 1 to 10 do
  begin
    Second1 := TPRandom(120);
    Second2 := Second1 + 20 + TPRandom(20);
    Minute1 := TPRandom(50) + 80;
    if ByteTemp = 1 then
    begin
      Second1 := 0;
      Second2 := 160;
      Minute1 := 130;
    end;
    for TmpB := Minute1 to 160 do
      for TmpA := Second1 to Second2 do
        PByteWindow(Background)^[Word(TmpB * ViewWidth + TmpA)] := $B0;
    if ByteTemp <> 1 then
      for TmpB := 1 to 20 do
      begin
        MainTmp := (Minute1 + TPRandom(160 - Minute1)) * ViewWidth;
        PByteWindow(Background)^[Word(MainTmp +
          (Second1 + TPRandom(Second2 - Second1)))] := $16;
      end;
  end;
  repeat
    Inc(FrameCount);
    { every 30 frames: measure the elapsed time (hundredths) and adapt the
      frame delay; update the destruction percentage }
    if FrameCount mod 30 = 0 then
    begin
      asm
        mov ax, $2C00
        int $21
        mov Second2, dh
        mov Hundredth2, dl
        mov Minute2, cl
      end ['AX', 'CX', 'DX'];
      TmpB := (Minute2 - Minute1) * 6000 + (Second2 - Second1) * 100 +
        Hundredth2 - Hundredth1;
      Percent := RTrunc(RMul(RDiv(RFromLong(Destroyed),
        RFromLong(BuildingCells)), MainHundred));
      if (TmpB > 125) and (FrameDelay > 0) then
        if FrameDelay > DelayStep then
          Dec(FrameDelay, DelayStep)
        else
          FrameDelay := 0;
      if TmpB < 120 then
        Inc(FrameDelay, DelayStep);
      Second1 := Second2;
      Hundredth1 := Hundredth2;
      Minute1 := Minute2;
    end;
    TPDelay(FrameDelay);
    if (Percent <> ShownPercent) or (BonusCollected <> BonusShown) then
      UpdateGoalCounters($CB0F); { screen offset 162 * 320 + 143 }
    while TPKeyPressed do
      LastKey := TPReadKey;
    if PlayerState[1] <> 0 then
    begin
      SetCamera(RenderEntries[0].XBits, RenderEntries[0].YBits);
      RenderView;
      ScreenOfs := ViewOfs[1];
      PresentView;
    end;
    if PlayerState[2] <> 0 then
    begin
      SetCamera(RenderEntries[1].XBits, RenderEntries[1].YBits);
      RenderView;
      ScreenOfs := ViewOfs[2];
      PresentView;
    end;
    Touches[1] := 0;
    Touches[2] := 0;
    Pickup[1] := 0;
    Pickup[2] := 0;
    { spawn points }
    if SpawnCount > 0 then
    begin
      GI := 1;
      while SpawnCount >= GI do
      begin
        with Spawns[GI] do
        begin
          Dec(Countdown);
          if (Countdown = 0) and (AvailableCount > 0) and
             (RemainingCount > 0) and (Enabled = 1) then
          begin
            Countdown := ReloadCount;
            { Anims[21..24] is the per-kind selector table (DS:0082) }
            ByteTemp := Anims[Anims[ActorKind + 20, 1], 1];
            SpawnActor(XBits, YBits, 0, 0, ByteTemp, ActorKind, 0, Behavior);
            if TmpA = 1 then
            begin
              Dec(RemainingCount);
              Dec(AvailableCount);
              with Actors[ExtraCount + 2] do
              begin
                Param0E := TPRandom(Param0ESpan) + Param0EBase;
                Param10 := TPRandom(Param10Span) + Param10Base;
                Param12 := TPRandom(Param12Span) + Param12Base;
                Energy := EnergyBase + TPRandom(EnergySpan);
                AnimationSelectorA := Anims[ActorKind + 20, 1];
                AnimationSelectorB := Anims[ActorKind + 20, 2];
                AnimationInit(Animation, ByteTemp,
                  Anims[Anims[ActorKind + 20, 1], 2], AnimationDelay, 1);
                OriginId := GI;
              end;
            end;
          end;
        end;
        Inc(GI);
      end;
    end;
    { players: bomb icons, lost lives, waiting to continue }
    for GI := 1 to 2 do
    begin
      if (BombIconDirty[GI] > 0) and (PlayerState[GI] = 1) then
        DrawBombIcon(HudX[GI] + 121, 163, GI);
      with Actors[GI] do
      begin
        if Behavior = 2 then
        begin
          Dec(Param10);
          if Param10 = 0 then
          begin
            Dec(Lives[GI]);
            if Lives[GI] = 255 then
            begin
              PlayerState[GI] := 0;
              Dec(Players);
              Inc(FrameDelay, 6);
              if Players = 1 then
                FillBox((GI - 1) * 160, 0, 160, 160, 0);
              RenderEntries[Actors[GI].SpriteSlot].XBits := 10000;
              if Players = 0 then
              begin
                EndGame(1);
                goto MenuLoop;
              end;
            end
            else
            begin
              PlaceAtStartPoint(Actors[GI], GI);
              DrawLives(GI);
              if Bombs[GI, 1] < 100 then
                Bombs[GI, 1] := 100;
              if Bombs[GI, 2] < 10 then
                Bombs[GI, 2] := 10;
              if Bombs[GI, 3] < 2 then
                Bombs[GI, 3] := 2;
              PlayerState[GI] := 2;
              with RenderEntries[GI - 1] do
              begin
                BitmapOffset := SpriteDefs[39].BitmapOffset;
                if CanContinue = 0 then
                begin
                  Width := 1;
                  Height := 1;
                end;
              end;
            end;
          end;
        end;
        if GI = 1 then
          ByteTemp := Keys[4]
        else
          ByteTemp := Keys[9];
        if CanContinue = 0 then
        begin
          ByteTemp := 0;
          AllDeadTimer := $E5;
        end;
        if PlayerState[GI] = 2 then
        begin
          with RenderEntries[Actors[GI].SpriteSlot] do
          begin
            TmpB := (Word(YBits + 7) shr 3 + 1) * MapWidth + XBits shr 3;
            if ((PByteWindow(MapA)^[TmpB] >= 1) and
                (PByteWindow(MapA)^[TmpB] <= 76)) or
               ((PByteWindow(MapA)^[Word(TmpB + 1)] >= 1) and
                (PByteWindow(MapA)^[Word(TmpB + 1)] <= 76) and
                (YBits > 24)) then
              Dec(YBits);
          end;
          if ByteTemp = 1 then
          begin
            if GI = 1 then
              Behavior := 0
            else
              Behavior := 1;
            PlayerState[GI] := 1;
            Keys[4] := 0;
            Keys[9] := 0;
            Actors[GI].Energy := 100;
          end;
        end;
      end;
    end;
    if MotionCount > 0 then
      UpdateMotions;
    GI := 1;
    while ExtraCount >= GI do
    begin
      UpdateActor(Actors[GI + 2]);
      Inc(GI);
    end;
    if (PlayerState[1] <> 1) and (PlayerState[2] <> 1) then
    begin
      Inc(AllDeadTimer);
      if AllDeadTimer = 230 then
      begin
        for GI := 1 to 2 do
          if PlayerState[GI] = 2 then
            PlayerState[GI] := 1;
        goto LevelStart;
      end;
    end
    else
      AllDeadTimer := 0;
    { active players: move, energy, energy bar and score display }
    for GI := 1 to 2 do
      if PlayerState[GI] = 1 then
      begin
        UpdateActor(Actors[GI]);
        if Actors[GI].Behavior <> 2 then
          Dec(Actors[GI].Energy, Touches[GI]);
        if Touches[GI] > 0 then
        begin
          TmpB := 45;
          SoundRequest := 4;
          RequestSound;
        end;
        TmpB := Actors[GI].Energy;
        if TmpB > 200 then
          PlayerDies(GI);
        if ShownEnergy[GI] <> TmpB then
        begin
          ShownEnergy[GI] := TmpB;
          TmpA := HudX[GI] + 1;
          DrawEnergyBar;
        end;
        if GI = 1 then
        begin
          TmpA := 0;
          ScorePtr := @ScoreDisp1;
        end
        else
        begin
          TmpA := ScoreGap;
          ScorePtr := @ScoreDisp2;
        end;
        if TScoreDisplay45(ScorePtr^).DisplayState < 2 then
        begin
          if TScoreDisplay45(ScorePtr^).DisplayState = 0 then
            SetScoreTargets(TScoreDisplay45(ScorePtr^));
          TmpB := HudX[GI] + $DC00; { + 176 * 320 }
          RollScoreDigits(TmpA);
          DrawScoreDigits(TmpA);
        end;
      end;
    if (ProjLast > 0) or (DebrisLast >= 200) then
      UpdateDebris;
    if GroupLast > 0 then
      UpdateGroups;
    if Shaking <> 0 then
    begin
      ShakeOffset := TPRandom(ShakeTimer * 2);
      Dec(ShakeTimer);
      if ShakeTimer = 0 then
      begin
        Shaking := 0;
        ShakeOffset := 0;
      end;
    end;
    { palette flashes: move each component 2 steps towards its goal }
    if FlashCount > 0 then
      for GI := 1 to FlashCount do
      begin
        Flag := 0;
        for ByteTemp := 1 to 3 do
          if FlashNowRGB[GI, ByteTemp] <> FlashGoalRGB[GI, ByteTemp] then
          begin
            Flag := 1;
            if FlashNowRGB[GI, ByteTemp] > FlashGoalRGB[GI, ByteTemp] then
              Dec(FlashNowRGB[GI, ByteTemp], 2)
            else
              Inc(FlashNowRGB[GI, ByteTemp], 2);
          end;
        if (Flag = 0) and (FlashCount = GI) then
          Dec(FlashCount);
        SetRGB(FlashColour[GI], FlashNow[GI].Red, FlashNow[GI].Green,
          FlashNow[GI].Blue);
      end;
    if BonusRain > 0 then
    begin
      Dec(BonusRain);
      DropBonusTile;
    end;
    if FrameCount mod 5 = 0 then
    begin
      SetFirePalette;
      FireLevel := FireLevel + 7;
      if FireLevel > 63 then
        FireLevel := 20;
    end;
    { development keys: S background on/off, E/R narrower/wider view }
    if LastKey = 's' then
    begin
      ShowBackground := Ord(ShowBackground = 0);
      LastKey := '9';
    end;
    if (LastKey = 'e') and (ViewTiles > 21) and (Players = 1) then
    begin
      FillBox(0, 0, 320, 160, 0);
      SetViewWidth(ViewTiles - 1);
      LastKey := '9';
      ViewTilesSaved := ViewTiles;
      DrawPlayfieldFrame;
    end;
    if (LastKey = 'r') and (ViewTiles < 40) and (Players = 1) then
    begin
      FillBox(0, 0, 320, 160, 0);
      SetViewWidth(ViewTiles + 1);
      LastKey := '9';
      ViewTilesSaved := ViewTiles;
      DrawPlayfieldFrame;
    end;
    { level completed }
    if (BonusDone <> 0) and (DestructionDone <> 0) and (GroupLast = 0) then
    begin
      LevelCompleted;
      if Level <= 7 then
        goto LevelStart
      else
      begin
        EndGame(2);
        goto MenuLoop;
      end;
    end;
  until LastKey = #27;
  EndGame(1);
  goto MenuLoop;
Finish:
  Shutdown(0);
end.
