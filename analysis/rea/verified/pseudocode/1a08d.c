/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a08d FUN_1920_0e8d
 * ev_a50b879d24e982b6848ca75112c3aa4f963d0c22284ba681e000d76ceed8f4e4
 */

undefined4 __cdecl16near FUN_1920_0e8d(void)

{
  uint uVar1;
  char cVar2;
  byte in_AL;
  uint uVar3;
  byte bVar4;
  char in_CH;
  undefined1 uVar5;
  uint in_DX;
  uint uVar6;
  uint in_BX;
  bool bVar7;
  
  cVar2 = -in_AL;
  uVar6 = in_DX;
  if (in_AL < 0xa0) {
    if (0x1f < (byte)(cVar2 + 0x9fU)) {
      return 0;
    }
    bVar4 = cVar2 + 0xa0;
    uVar6 = in_DX | 0x8000;
    if (0x10 < bVar4) {
      uVar6 = 0;
      bVar4 = cVar2 + 0x90;
      in_BX = in_DX | 0x8000;
    }
    uVar3 = in_BX;
    if (8 < bVar4) {
      uVar5 = (undefined1)uVar6;
      uVar6 = uVar6 >> 8;
      bVar4 = bVar4 - 8;
      uVar3 = CONCAT11(uVar5,(char)(in_BX >> 8));
    }
    do {
      uVar1 = uVar6 & 1;
      uVar6 = uVar6 >> 1;
      in_BX = (uint)(CONCAT12(uVar1 != 0,uVar3) >> 1);
      uVar1 = uVar3 & 1;
      bVar4 = bVar4 - 1;
      uVar3 = in_BX;
    } while (bVar4 != 0);
    if ((uVar1 != 0) && (in_CH != '\0')) {
      bVar7 = 0xfffe < in_BX;
      in_BX = in_BX + 1;
      uVar6 = uVar6 + bVar7;
      if ((int)uVar6 < 0) goto LAB_1920_0ee2;
    }
    if ((int)in_DX < 0) {
      uVar3 = ~in_BX;
      in_BX = uVar3 + 1;
      uVar6 = ~uVar6 + (uint)(0xfffe < uVar3);
    }
    return CONCAT22(uVar6,in_BX);
  }
LAB_1920_0ee2:
  return CONCAT22(uVar6,in_BX);
}


