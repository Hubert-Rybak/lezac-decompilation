/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a15d FUN_1920_0f5d
 * ev_f0a5b83242106049558ed3bb07f3cd026850734f9d273beef108d8160e7f7f0e
 */

uint __cdecl16far FUN_1920_0f5d(void)

{
  uint uVar1;
  byte bVar2;
  uint in_AX;
  uint uVar3;
  byte bVar4;
  uint uVar5;
  undefined1 uVar7;
  uint uVar6;
  
  bVar2 = (byte)in_AX;
  if (bVar2 < 0xa8) {
    uVar3 = in_AX & 0xff;
    bVar4 = bVar2 + 0x80;
    uVar1 = 0;
    uVar6 = 0;
    if (bVar2 < 0x80 || bVar4 == 0) {
      return 0;
    }
    while( true ) {
      uVar5 = uVar1;
      uVar7 = (undefined1)(uVar6 >> 8);
      if (bVar4 < 0x10) break;
      uVar3 = CONCAT11(uVar7,(char)uVar3);
      bVar4 = bVar4 - 0x10;
      uVar1 = 0xffff;
      uVar6 = uVar5;
    }
    if (7 < bVar4) {
      uVar3 = CONCAT11((char)uVar6,(char)uVar3);
      uVar6 = CONCAT11((char)uVar5,uVar7);
      uVar5 = CONCAT11(0xff,(char)(uVar5 >> 8));
      bVar4 = bVar4 - 8;
    }
    for (; bVar4 != 0; bVar4 = bVar4 - 1) {
      uVar1 = uVar5 & 1;
      uVar3 = CONCAT11((char)(((uint)((uVar6 & 1) != 0) << 8 | uVar3 >> 8) >> 1),(char)uVar3);
      uVar5 = (uint)(((ulong)uVar5 | 0x10000) >> 1);
      uVar6 = (uint)(CONCAT12(uVar1 != 0,uVar6) >> 1);
    }
    in_AX = CONCAT11((byte)(uVar3 >> 8) & (byte)(in_AX >> 8),(char)uVar3);
  }
  return in_AX;
}


