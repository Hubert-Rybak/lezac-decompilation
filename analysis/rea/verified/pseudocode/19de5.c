/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19de5 FUN_1920_0be5
 * ev_b20cc2c84a15d3911eaa1792de74bd2ac099d05f0ffc9220d6404c81231fc3bb
 */

undefined4 __cdecl16near FUN_1920_0be5(void)

{
  ulong uVar1;
  byte bVar2;
  uint in_AX;
  uint uVar3;
  char cVar4;
  uint in_CX;
  uint in_DX;
  uint uVar5;
  uint uVar6;
  uint in_BX;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint unaff_SI;
  uint unaff_DI;
  uint uVar11;
  bool bVar12;
  undefined4 uVar13;
  
  if ((byte)in_CX != 0) {
    if ((byte)in_AX == 0) {
LAB_1920_0c65:
      return CONCAT22(unaff_DI,in_CX);
    }
    uVar10 = in_AX;
    uVar5 = in_DX;
    uVar7 = in_BX;
    if ((byte)in_CX < (byte)in_AX) {
      uVar10 = in_CX;
      in_CX = in_AX;
      uVar5 = unaff_DI;
      uVar7 = unaff_SI;
      unaff_SI = in_BX;
      unaff_DI = in_DX;
    }
    bVar2 = -((char)uVar10 - (char)in_CX);
    if (0x27 < bVar2) goto LAB_1920_0c65;
    uVar3 = CONCAT11((char)(uVar5 >> 8),(char)in_CX) & 0x80ff;
    uVar10 = uVar10 & 0xff00;
    uVar5 = uVar5 | 0x8000;
    uVar11 = unaff_DI | 0x8000;
    for (; 7 < bVar2; bVar2 = bVar2 - 8) {
      uVar10 = CONCAT11((char)uVar7,(char)(uVar10 >> 8));
      uVar7 = CONCAT11((char)uVar5,(char)(uVar7 >> 8));
      uVar5 = uVar5 >> 8;
    }
    uVar6 = CONCAT11((char)(in_CX >> 8),bVar2);
    while (bVar2 != 0) {
      uVar9 = uVar5 & 1;
      uVar5 = uVar5 >> 1;
      uVar10 = (uint)(CONCAT12((uVar7 & 1) != 0,uVar10) >> 1);
      bVar2 = (char)uVar6 - 1;
      uVar6 = CONCAT11((char)(uVar6 >> 8),bVar2);
      uVar7 = (uint)(CONCAT12(uVar9 != 0,uVar7) >> 1);
    }
    if ((int)(uVar3 ^ unaff_DI) < 0) {
      in_AX = uVar10 - uVar6;
      uVar9 = (uVar7 - unaff_SI) - (uint)(uVar10 < uVar6);
      uVar10 = (uint)(uVar7 < unaff_SI || uVar7 - unaff_SI < (uint)(uVar10 < uVar6));
      in_DX = (uVar5 - uVar11) - uVar10;
      if (uVar5 < uVar11 || uVar5 - uVar11 < uVar10) {
        uVar10 = ~uVar9;
        bVar12 = in_AX == 0;
        in_AX = -in_AX;
        uVar9 = uVar10 + bVar12;
        in_DX = ~in_DX + (uint)CARRY2(uVar10,(uint)bVar12);
        uVar3 = uVar3 ^ 0x8000;
      }
      if ((in_DX == 0 && uVar9 == 0) && in_AX == 0) goto LAB_1920_0c64;
      while (-1 < (int)in_DX) {
        bVar12 = (int)in_AX < 0;
        in_AX = in_AX << 1;
        uVar1 = (ulong)CONCAT12(bVar12,uVar9) << 1;
        uVar9 = (uint)uVar1 | (uint)bVar12;
        in_DX = in_DX << 1 | (uint)((uVar1 & 0x10000) != 0);
        cVar4 = (char)uVar3 + -1;
        uVar3 = CONCAT11((char)(uVar3 >> 8),cVar4);
        if (cVar4 == '\0') {
          uVar13 = FUN_1920_0da6();
          return uVar13;
        }
      }
    }
    else {
      in_AX = uVar10 + uVar6;
      uVar8 = uVar7 + unaff_SI + (uint)CARRY2(uVar10,uVar6);
      uVar10 = (uint)(CARRY2(uVar7,unaff_SI) || CARRY2(uVar7 + unaff_SI,(uint)CARRY2(uVar10,uVar6)))
      ;
      uVar7 = uVar5 + uVar11;
      uVar6 = uVar7 + uVar10;
      in_DX = uVar6;
      uVar9 = uVar8;
      if (CARRY2(uVar5,uVar11) || CARRY2(uVar7,uVar10)) {
        in_DX = (uint)(CONCAT12(CARRY2(uVar5,uVar11) || CARRY2(uVar7,uVar10),uVar6) >> 1);
        uVar9 = (uint)(CONCAT12((uVar6 & 1) != 0,uVar8) >> 1);
        in_AX = (uint)(CONCAT12((uVar8 & 1) != 0,in_AX) >> 1);
        cVar4 = (char)uVar3 + '\x01';
        uVar3 = CONCAT11((char)(uVar3 >> 8),cVar4);
        if (cVar4 == '\0') goto LAB_1920_0c72;
      }
    }
    bVar12 = 0xff7f < in_AX;
    in_AX = in_AX + 0x80;
    uVar10 = (uint)CARRY2(uVar9,(uint)bVar12);
    bVar12 = CARRY2(in_DX,uVar10);
    in_DX = in_DX + uVar10;
    if (bVar12) {
      in_DX = (uint)(CONCAT12(1,in_DX) >> 1);
      cVar4 = (char)uVar3 + '\x01';
      uVar3 = CONCAT11((char)(uVar3 >> 8),cVar4);
      if (cVar4 == '\0') {
LAB_1920_0c72:
        return CONCAT22(in_DX,in_AX);
      }
    }
    in_AX = CONCAT11((char)(in_AX >> 8),(char)uVar3);
    in_DX = CONCAT11((byte)((in_DX & 0x7fff) >> 8) | (byte)(uVar3 >> 8),(char)(in_DX & 0x7fff));
  }
LAB_1920_0c64:
  return CONCAT22(in_DX,in_AX);
}


