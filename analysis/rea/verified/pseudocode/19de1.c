/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19de1 FUN_1920_0be1
 * ev_aa0064dabb9019a7bb11ede40752d84cb84caacf9be48c3c4a81aac557f2ab52
 */

undefined4 FUN_1920_0be1(void)

{
  ulong uVar1;
  byte bVar2;
  uint in_AX;
  uint uVar3;
  uint uVar4;
  char cVar5;
  uint in_CX;
  uint uVar6;
  uint in_DX;
  uint uVar7;
  uint uVar8;
  uint in_BX;
  uint uVar9;
  uint uVar10;
  uint unaff_SI;
  uint unaff_DI;
  uint uVar11;
  uint uVar12;
  bool bVar13;
  undefined4 uVar14;
  
  uVar11 = unaff_DI ^ 0x8000;
  if ((byte)in_CX != 0) {
    if ((byte)in_AX == 0) {
LAB_1920_0c65:
      return CONCAT22(uVar11,in_CX);
    }
    uVar4 = in_AX;
    uVar7 = in_DX;
    uVar8 = in_BX;
    if ((byte)in_CX < (byte)in_AX) {
      uVar4 = in_CX;
      in_CX = in_AX;
      uVar7 = uVar11;
      uVar8 = unaff_SI;
      unaff_SI = in_BX;
      uVar11 = in_DX;
    }
    bVar2 = -((char)uVar4 - (char)in_CX);
    if (0x27 < bVar2) goto LAB_1920_0c65;
    uVar3 = CONCAT11((char)(uVar7 >> 8),(char)in_CX) & 0x80ff;
    uVar4 = uVar4 & 0xff00;
    uVar7 = uVar7 | 0x8000;
    uVar12 = uVar11 | 0x8000;
    for (; 7 < bVar2; bVar2 = bVar2 - 8) {
      uVar4 = CONCAT11((char)uVar8,(char)(uVar4 >> 8));
      uVar8 = CONCAT11((char)uVar7,(char)(uVar8 >> 8));
      uVar7 = uVar7 >> 8;
    }
    uVar6 = CONCAT11((char)(in_CX >> 8),bVar2);
    while (bVar2 != 0) {
      uVar10 = uVar7 & 1;
      uVar7 = uVar7 >> 1;
      uVar4 = (uint)(CONCAT12((uVar8 & 1) != 0,uVar4) >> 1);
      bVar2 = (char)uVar6 - 1;
      uVar6 = CONCAT11((char)(uVar6 >> 8),bVar2);
      uVar8 = (uint)(CONCAT12(uVar10 != 0,uVar8) >> 1);
    }
    if ((int)(uVar3 ^ uVar11) < 0) {
      in_AX = uVar4 - uVar6;
      uVar10 = (uVar8 - unaff_SI) - (uint)(uVar4 < uVar6);
      uVar11 = (uint)(uVar8 < unaff_SI || uVar8 - unaff_SI < (uint)(uVar4 < uVar6));
      in_DX = (uVar7 - uVar12) - uVar11;
      if (uVar7 < uVar12 || uVar7 - uVar12 < uVar11) {
        uVar11 = ~uVar10;
        bVar13 = in_AX == 0;
        in_AX = -in_AX;
        uVar10 = uVar11 + bVar13;
        in_DX = ~in_DX + (uint)CARRY2(uVar11,(uint)bVar13);
        uVar3 = uVar3 ^ 0x8000;
      }
      if ((in_DX == 0 && uVar10 == 0) && in_AX == 0) goto LAB_1920_0c64;
      while (-1 < (int)in_DX) {
        bVar13 = (int)in_AX < 0;
        in_AX = in_AX << 1;
        uVar1 = (ulong)CONCAT12(bVar13,uVar10) << 1;
        uVar10 = (uint)uVar1 | (uint)bVar13;
        in_DX = in_DX << 1 | (uint)((uVar1 & 0x10000) != 0);
        cVar5 = (char)uVar3 + -1;
        uVar3 = CONCAT11((char)(uVar3 >> 8),cVar5);
        if (cVar5 == '\0') {
          uVar14 = FUN_1920_0da6();
          return uVar14;
        }
      }
    }
    else {
      in_AX = uVar4 + uVar6;
      uVar9 = uVar8 + unaff_SI + (uint)CARRY2(uVar4,uVar6);
      uVar11 = (uint)(CARRY2(uVar8,unaff_SI) || CARRY2(uVar8 + unaff_SI,(uint)CARRY2(uVar4,uVar6)));
      uVar4 = uVar7 + uVar12;
      uVar8 = uVar4 + uVar11;
      in_DX = uVar8;
      uVar10 = uVar9;
      if (CARRY2(uVar7,uVar12) || CARRY2(uVar4,uVar11)) {
        in_DX = (uint)(CONCAT12(CARRY2(uVar7,uVar12) || CARRY2(uVar4,uVar11),uVar8) >> 1);
        uVar10 = (uint)(CONCAT12((uVar8 & 1) != 0,uVar9) >> 1);
        in_AX = (uint)(CONCAT12((uVar9 & 1) != 0,in_AX) >> 1);
        cVar5 = (char)uVar3 + '\x01';
        uVar3 = CONCAT11((char)(uVar3 >> 8),cVar5);
        if (cVar5 == '\0') goto LAB_1920_0c72;
      }
    }
    bVar13 = 0xff7f < in_AX;
    in_AX = in_AX + 0x80;
    uVar11 = (uint)CARRY2(uVar10,(uint)bVar13);
    bVar13 = CARRY2(in_DX,uVar11);
    in_DX = in_DX + uVar11;
    if (bVar13) {
      in_DX = (uint)(CONCAT12(1,in_DX) >> 1);
      cVar5 = (char)uVar3 + '\x01';
      uVar3 = CONCAT11((char)(uVar3 >> 8),cVar5);
      if (cVar5 == '\0') {
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


