unit LZGRAPH;
{ Reconstruction of the original executable's graphics/map unit: code segment
  08AC (REA 0x18AC0..0x191FF), data DS:7F70..C49D.

  The original unit holds the tile set, sprite store, active render list, the
  scrolling-view geometry and the mode 13h renderer used by the main program.
  Procedure and variable NAMES are descriptive choices, not recovered Borland
  identifiers. The rendering procedures were assembly in the original
  (no Pascal frame, register-level loops); they are reproduced as assembly
  with the same instruction-level behaviour, addressing the variables by name
  instead of by the original DS offsets given in the comments. }
{$IFDEF FPC}{$MODE TP}{$ASMMODE INTEL}{$ENDIF}
{$R-}{$Q-}{$S-}{$I-}

interface

uses LZTYPES;

type
  { The tile pixels and their transparency masks are one 17,004-byte block in
    the original: the renderer reaches a mask at pixel address + $2136. }
  TTileStore = packed record
    Pixels: array[0..8501] of Byte;   { DS:7F74, 64 bytes per 8x8 tile }
    Masks: array[0..8501] of Byte;    { DS:A0AA, 0FFh where the pixel is 0 }
  end;

var
  Screen: Pointer;          { DS:7F70  57,000-byte back buffer (main program) }
  Tiles: TTileStore;        { DS:7F74 }
  MapA: Pointer;            { DS:C1E0  tile-code map, offset 0 after loading }
  SpriteData: Pointer;      { DS:C1E4  sprite bitmap store }
  GTemp: Word;              { DS:C1E8  shared index; also used by the program }
  ViewTiles: Word;          { DS:C1EA  view width in tiles }
  ViewWidth: Word;          { DS:C1EC  view width in pixels (back-buffer pitch) }
  ViewHeight: Word;         { DS:C1EE  view height in pixels }
  MapViewOfs: Word;         { DS:C1F0  first visible map cell }
  ScreenOfs: Word;          { DS:C1F4  video offset of the view }
  BackgroundSkip: Word;     { DS:C1F6  background bytes skipped per row }
  BackgroundPitch: Word;    { DS:C1F8  background bytes per text row group }
  SpriteSeg: Word;          { DS:C1FA }
  RenderTableOfs: Word;     { DS:C1FC  Ofs(RenderEntries) }
  MapSeg: Word;             { DS:C1FE  Seg(MapA^) }
  SpriteH: Word;            { DS:C200 }
  SpriteW: Word;            { DS:C202 }
  MapWidth: Word;           { DS:C204  map width in cells }
  ScreenSkip: Word;         { DS:C206  320 - view width }
  TileBase: Word;           { DS:C208  Ofs(Tiles) }
  FineX: Word;              { DS:C20A  sub-tile scroll, pixels }
  FineY: Word;              { DS:C20C }
  RowTemp: Word;            { DS:C210  tile row / clipped source skip }
  BackSeg: Word;            { DS:C212  Seg(Screen^) }
  BackOfs: Word;            { DS:C214  Ofs(Screen^) }
  CamX: Word;               { DS:C216  view origin in map pixels }
  CamY: Word;               { DS:C218 }
  SpriteX: Integer;         { DS:C21A  sprite position relative to the view }
  SpriteY: Integer;         { DS:C21C }
  RenderEntries: array[0..32] of TRenderSprite8;  { DS:C21E }
  SpriteDefs: array[1..92] of TSpriteDef4;        { DS:C326 }
  SpriteCount: Byte;        { DS:C496  active render entries }
  Background: Pointer;      { DS:C498  60,000-byte background image }
  ShowBackground: Byte;     { DS:C49C }

procedure SetRGB(Index, Red, Green, Blue: Byte);      { 18AC0 }
procedure MakeBackground;                             { 18AE2 }
procedure RenderView;                                 { 18BB4 }
procedure PresentView;                                { 18E88 }
procedure InitGraph;                                  { 18ED7 }
procedure LoadTiles(Name: string);                    { 18EE2 }
procedure AddSprite(Sprite, X, Y: Word);              { 18FD7 }
procedure RemoveSprite(Index: Integer);               { 19054 }
procedure LoadSprites(Name: string);                  { 1909C }

implementation

type
  TBytes = array[0..65534] of Byte;
  PBytes = ^TBytes;

procedure SetRGB(Index, Red, Green, Blue: Byte);
begin
  asm
    mov dx, $3C8
    mov al, Index
    out dx, al
    inc dx
    mov al, Red
    out dx, al
    mov al, Green
    out dx, al
    mov al, Blue
    out dx, al
  end;
end;

{ Sky gradient: DAC colours $B0..$D6 and one colour per band of rows. }
procedure MakeBackground;
var
  C: Byte;
  R0, G0, B0, DR, DG, DB: ShortInt;
begin
  R0 := 0;
  G0 := 0;
  B0 := 14;
  DR := 43;
  DG := 23;
  DB := -12;
  for C := $B0 to $D6 do
    SetRGB(C, R0 + (C - $B0) * DR div 38, G0 + (C - $B0) * DG div 38,
           B0 + (C - $B0) * DB div 38);
  for GTemp := 0 to 59999 do
    PBytes(Background)^[GTemp] := GTemp div (ViewWidth * 4) + $B0;
end;

{ Background, tile layer and sprites into the back buffer (assembly). }
procedure RenderView; assembler;
asm
  mov al, ShowBackground
  cmp al, 1
  jne @tiles
  push ds
  les di, Background
  mov ax, CamX
  add ax, FineX
  mov bx, 4
  xor dx, dx
  div bx
  add di, ax
  mov ax, CamY
  add ax, FineY
  shr ax, 1
  shr ax, 1
  shr ax, 1
  mov bx, BackgroundPitch
  mul bx
  add di, ax
  push es
  push di
  mov di, BackOfs
  mov es, BackSeg
  add di, FineX
  mov bx, ViewWidth
  mov ax, FineY
  mul bx
  add di, ax
  mov ax, ViewWidth
  mov bx, ViewHeight
  shr ax, 1
  mov dx, BackgroundSkip
  pop si
  pop ds
  mov cx, bx
@bgrow:
  mov bx, cx
  mov cx, ax
  rep movsw
  mov cx, bx
  add si, dx
  loop @bgrow
  pop ds
@tiles:
  mov di, BackOfs
  mov es, BackSeg
  mov si, MapViewOfs
  mov bx, 64
  mov cx, 20
  mov al, ShowBackground
  cmp al, 1
  jne @opaque
@trow:
  push cx
  mov ax, TileBase
  mov RowTemp, ax
  mov cx, 8
@tline:
  push si
  push cx
  mov cx, ViewTiles
@tcell:
  push ds
  mov ds, MapSeg
  lodsb
  pop ds
  xor ah, ah
  mul bx
  add ax, RowTemp
  push si
  mov si, ax
  push cx
  mov cx, 4
@tword:
  mov ax, word ptr [si + 8502]
  and es:[di], ax
  lodsw
  or es:[di], ax
  add di, 2
  loop @tword
  pop cx
  pop si
  loop @tcell
  mov ax, RowTemp
  add ax, 8
  mov RowTemp, ax
  pop cx
  pop si
  loop @tline
  add si, MapWidth
  pop cx
  loop @trow
  jmp @sprites
@opaque:
  push cx
  mov ax, TileBase
  mov RowTemp, ax
  mov cx, 8
@oline:
  push si
  push cx
  mov cx, ViewTiles
@ocell:
  push ds
  mov ds, MapSeg
  lodsb
  pop ds
  xor ah, ah
  mul bx
  add ax, RowTemp
  push si
  mov si, ax
  mov ax, cx
  mov cx, 4
  rep movsw
  mov cx, ax
  pop si
  loop @ocell
  mov ax, RowTemp
  add ax, 8
  mov RowTemp, ax
  pop cx
  pop si
  loop @oline
  add si, MapWidth
  pop cx
  loop @opaque
@sprites:
  mov cl, SpriteCount
  xor ch, ch
  cmp cx, 0
  jne @first
  jmp @done
@first:
  xor bx, bx
@entry:
  mov al, byte ptr [bx + RenderEntries + 4]
  xor ah, ah
  mov SpriteW, ax
  mov al, byte ptr [bx + RenderEntries + 5]
  mov SpriteH, ax
  mov ax, word ptr [bx + RenderEntries]
  sub ax, CamX
  mov SpriteX, ax
  mov ax, word ptr [bx + RenderEntries + 2]
  sub ax, CamY
  mov SpriteY, ax
  cmp ax, ViewHeight
  jl @v1
  jmp @next
@v1:
  add ax, SpriteH
  cmp ax, 0
  jg @v2
  jmp @next
@v2:
  mov ax, SpriteX
  cmp ax, ViewWidth
  jl @v3
  jmp @next
@v3:
  add ax, SpriteW
  cmp ax, 0
  jle @next
  mov ax, SpriteY
  cmp ax, -1
  jg @v4
  jmp @clipped
@v4:
  add ax, SpriteH
  dec ax
  cmp ax, ViewHeight
  jge @clipped
  mov ax, SpriteX
  cmp ax, -1
  jle @clipped
  add ax, SpriteW
  dec ax
  cmp ax, ViewWidth
  jge @clipped
  { entirely inside the view }
  push cx
  mov es, BackSeg
  mov di, BackOfs
  mov ax, SpriteY
  mov cx, bx
  mov bx, ViewWidth
  mul bx
  mov bx, cx
  add ax, SpriteX
  add di, ax
  mov si, word ptr [bx + RenderEntries + 6]
  mov ax, ViewWidth
  sub ax, SpriteW
  mov GTemp, ax
  mov cx, SpriteH
  mov dx, SpriteW
  push bx
  mov bx, GTemp
  push ds
  mov ds, SpriteSeg
@srow:
  push cx
  mov cx, dx
@spix:
  lodsb
  cmp al, 0
  jne @sput
  inc di
  jmp @snext
@sput:
  stosb
@snext:
  loop @spix
  pop cx
  add di, bx
  loop @srow
  pop ds
  pop bx
  pop cx
@next:
  add bx, 8
  dec cx
  jcxz @last
  jmp @entry
@last:
  jmp @done
@clipped:
  push cx
  mov RowTemp, 0
  mov es, BackSeg
  mov di, BackOfs
  mov ax, SpriteY
  mov si, word ptr [bx + RenderEntries + 6]
  mov ax, SpriteY
  cmp ax, 0
  jge @cx1
  mov cx, bx
  mov bx, SpriteW
  neg ax
  sub SpriteH, ax
  mul bx
  add si, ax
  mov bx, cx
@cx1:
  mov ax, SpriteX
  cmp ax, 0
  jge @cx2
  neg ax
  add si, ax
  mov RowTemp, ax
  jmp @cx3
@cx2:
  add ax, SpriteW
  cmp ax, ViewWidth
  jle @cx3
  sub ax, ViewWidth
  mov RowTemp, ax
@cx3:
  mov ax, SpriteY
  add ax, SpriteH
  sub ax, ViewHeight
  cmp ax, -1
  jle @cx4
  mov cx, ax
  mov ax, SpriteH
  sub ax, cx
  mov SpriteH, ax
@cx4:
  mov ax, SpriteW
  sub ax, RowTemp
  mov SpriteW, ax
  mov ax, SpriteY
  cmp ax, 0
  jge @cx5
  mov SpriteY, 0
@cx5:
  mov ax, SpriteX
  cmp ax, 0
  jge @cx6
  mov SpriteX, 0
@cx6:
  mov ax, SpriteY
  mov cx, bx
  mov bx, ViewWidth
  mul bx
  mov bx, cx
  add ax, SpriteX
  add di, ax
  mov ax, ViewWidth
  sub ax, SpriteW
  mov GTemp, ax
  mov cx, SpriteH
@crow:
  push cx
  mov cx, SpriteW
  push ds
  mov ds, SpriteSeg
@cpix:
  lodsb
  cmp al, 0
  jne @cput
  inc di
  jmp @cnext
@cput:
  stosb
@cnext:
  loop @cpix
  pop ds
  pop cx
  add di, GTemp
  add si, RowTemp
  loop @crow
  pop cx
  jmp @next
@done:
end;

{ Back buffer to video memory, skipping the 8-pixel scroll margin. The
  original loads DX with the VGA status port but never polls it. }
procedure PresentView; assembler;
asm
  mov dx, $3DA
  push ds
  mov ax, $A000
  mov es, ax
  mov di, ScreenOfs
  mov si, BackOfs
  mov ax, ViewWidth
  mov bx, FineY
  mul bx
  add ax, FineX
  add si, ax
  mov dx, ViewWidth
  sub dx, 8
  shr dx, 1
  mov bx, ScreenSkip
  add bx, 8
  mov cx, ViewHeight
  sub cx, 8
  mov ds, BackSeg
@row:
  mov ax, cx
  mov cx, dx
  rep movsw
  add di, bx
  add si, 8
  mov cx, ax
  loop @row
  pop ds
  mov dx, $3DA
end;

procedure InitGraph; assembler;
asm
  mov ax, $13
  int $10
  mov ax, $0F00
  int $10
end;

{ CARO.CAR: one unused byte, a tile count, then 64 bytes per tile. }
procedure LoadTiles(Name: string);
var
  FileName: string;
  F: file;
  Unused, Count: Byte;
  I, Size: Word;
begin
  FileName := Name;
  Assign(F, FileName);
  Reset(F, 1);
  BlockRead(F, Unused, 1, I);
  BlockRead(F, Count, 1, I);
  Size := Count shl 6;
  BlockRead(F, Tiles.Pixels, Size, I);
  for I := 0 to Size do
    if Tiles.Pixels[I] = 0 then
      Tiles.Masks[I] := $FF
    else
      Tiles.Masks[I] := 0;
  Close(F);
end;

procedure AddSprite(Sprite, X, Y: Word);
begin
  if SpriteCount <= 32 then
  begin
    with RenderEntries[SpriteCount] do
    begin
      XBits := X;
      YBits := Y;
      BitmapOffset := SpriteDefs[Sprite].BitmapOffset;
      Width := SpriteDefs[Sprite].Width;
      Height := SpriteDefs[Sprite].Height;
    end;
    Inc(SpriteCount);
  end;
end;

procedure RemoveSprite(Index: Integer);
begin
  if SpriteCount > 0 then
  begin
    if SpriteCount - 1 > Index then
      asm
        push ds
        pop es
        mov al, SpriteCount
        xor ah, ah
        sub ax, Index
        dec ax
        mov bx, 8
        mul bx
        mov cx, ax
        mov di, RenderTableOfs
        mov ax, Index
        mul bx
        add di, ax
        mov si, di
        add si, 8
        rep movsb
      end;
    Dec(SpriteCount);
  end;
end;

{ *.SPR: a count, then width, height and width*height pixels per sprite. }
procedure LoadSprites(Name: string);
var
  FileName: string;
  F: file;
  I: Integer;
  Count, W, H: Byte;
  Res, Offset, Size: Word;
begin
  FileName := Name;
  Assign(F, FileName);
  Reset(F, 1);
  Offset := 0;
  BlockRead(F, Count, 1, Res);
  for I := 1 to Count do
  begin
    BlockRead(F, W, 1, Res);
    BlockRead(F, H, 1, Res);
    SpriteDefs[I].Width := W;
    SpriteDefs[I].Height := H;
    Size := H * W;
    BlockRead(F, PBytes(SpriteData)^[Offset], Size, Res);
    SpriteDefs[I].BitmapOffset := Ofs(PBytes(SpriteData)^[Offset]);
    Offset := Offset + Size;
  end;
  Close(F);
end;

end.
