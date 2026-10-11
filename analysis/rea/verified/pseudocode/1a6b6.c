/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a6b6 FUN_1920_14b6
 * ev_bf04e04d3b71e8458f21b05944339351ef226cab373a2d7d2f661ed252783e8f
 */

undefined4 __cdecl16near FUN_1920_14b6(void)

{
  ulong uVar1;
  bool bVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int in_CX;
  uint uVar8;
  uint uVar9;
  byte bVar10;
  byte bVar11;
  char cVar12;
  byte *unaff_DI;
  undefined2 unaff_ES;
  bool bVar13;
  
  uVar3 = 0;
  uVar8 = 0;
  bVar2 = false;
  if (in_CX == 0) goto LAB_1920_1517;
  if (*unaff_DI == 0x2b) {
LAB_1920_14cb:
    unaff_DI = unaff_DI + 1;
    in_CX = in_CX + -1;
    if (in_CX == 0) goto LAB_1920_1517;
  }
  else if (*unaff_DI == 0x2d) {
    bVar2 = true;
    goto LAB_1920_14cb;
  }
  if (*unaff_DI == 0x24) {
    iVar5 = in_CX + -1;
    if (iVar5 == 0) {
LAB_1920_1517:
      return CONCAT22(uVar8,uVar3);
    }
    do {
      unaff_DI = unaff_DI + 1;
      bVar10 = *unaff_DI;
      if (0x60 < bVar10) {
        bVar10 = bVar10 - 0x20;
      }
      bVar11 = bVar10 - 0x30;
      if ((byte)(bVar10 - 0x3a) < 0xf6) {
        if ((byte)(bVar10 + 0xb9) < 0xfa) break;
        bVar11 = bVar10 - 0x37;
      }
      cVar12 = '\x04';
      do {
        bVar13 = (int)uVar3 < 0;
        uVar3 = uVar3 << 1;
        uVar1 = (ulong)CONCAT12(bVar13,uVar8) << 1;
        uVar8 = (uint)uVar1 | (uint)bVar13;
        if ((uVar1 & 0x10000) != 0) goto LAB_1920_1517;
        cVar12 = cVar12 + -1;
      } while (cVar12 != '\0');
      uVar3 = CONCAT11((char)(uVar3 >> 8),(byte)uVar3 | bVar11);
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
  }
  else {
    do {
      uVar7 = (uint)(byte)(*unaff_DI - 0x30);
      if ((byte)(*unaff_DI - 0x3a) < 0xf6) break;
      if ((uVar8 & 0xf000) != 0) goto LAB_1920_1517;
      uVar4 = uVar3 * 2;
      uVar9 = uVar8 << 1 | (uint)((int)uVar3 < 0);
      iVar5 = uVar3 << 2;
      uVar6 = uVar3 * 8;
      uVar8 = uVar3 * 10;
      uVar3 = uVar8 + uVar7;
      uVar8 = ((uVar9 << 1 | (uint)((int)uVar4 < 0)) << 1 | (uint)(iVar5 < 0)) + uVar9 +
              (uint)CARRY2(uVar6,uVar4) + (uint)CARRY2(uVar8,uVar7);
      if ((int)uVar8 < 0) goto LAB_1920_1517;
      unaff_DI = unaff_DI + 1;
      in_CX = in_CX + -1;
    } while (in_CX != 0);
  }
  if (bVar2) {
    uVar7 = ~uVar3;
    uVar3 = uVar7 + 1;
    uVar8 = ~uVar8 + (uint)(0xfffe < uVar7);
  }
  return CONCAT22(uVar8,uVar3);
}


