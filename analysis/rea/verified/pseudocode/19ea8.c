/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19ea8 FUN_1920_0ca8
 * ev_64f1d0704d5f906fde48a8e61ce5bad97a44290e69ac30be74b5d23015b17fac
 */

undefined4 __cdecl16near FUN_1920_0ca8(void)

{
  long lVar1;
  long lVar2;
  ulong uVar3;
  long lVar4;
  byte bVar5;
  undefined2 in_AX;
  byte bVar8;
  uint uVar6;
  uint uVar7;
  byte bVar9;
  uint in_CX;
  int iVar10;
  uint in_DX;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  int iVar15;
  uint in_BX;
  uint uVar16;
  uint uVar17;
  uint uVar18;
  uint uVar19;
  uint uVar20;
  uint uVar21;
  uint unaff_SI;
  uint unaff_DI;
  uint uVar22;
  uint uVar23;
  uint uVar24;
  bool bVar25;
  undefined4 uVar26;
  
  bVar5 = (byte)in_AX;
  bVar8 = (byte)((uint)in_AX >> 8);
  if ((bVar5 == 0) || (bVar9 = (byte)in_CX, bVar9 == 0)) {
    uVar26 = FUN_1920_0da6();
    return uVar26;
  }
  iVar10 = CONCAT11((((byte)(in_DX >> 8) ^ (byte)(unaff_DI >> 8)) & 0x80) + CARRY1(bVar5,bVar9),
                    bVar5 + bVar9);
  bVar5 = (byte)(in_CX >> 8);
  uVar21 = in_DX | 0x8000;
  uVar22 = unaff_DI | 0x8000;
  if (((bVar8 == 0) &&
      (uVar6 = in_CX & 0xff00, uVar7 = uVar21, uVar11 = unaff_SI, uVar23 = uVar22, in_BX == 0)) ||
     ((bVar5 == 0 &&
      (uVar6 = (uint)bVar8 << 8, uVar7 = uVar22, uVar11 = in_BX, uVar23 = uVar21, unaff_SI == 0))))
  {
    uVar3 = (ulong)uVar7 * (ulong)uVar11 + (ulong)((uVar7 >> 8) * (uVar6 >> 8));
    uVar16 = (uint)uVar3;
    uVar3 = (ulong)uVar7 * (ulong)uVar23 + (uVar3 >> 0x10);
  }
  else {
    lVar1 = (ulong)bVar8 * 0x100 * (ulong)unaff_SI;
    lVar4 = lVar1 + (ulong)((uint)bVar8 * (uint)bVar5);
    uVar23 = (uint)((ulong)lVar4 >> 0x10);
    lVar2 = (ulong)in_BX * (ulong)bVar5 * 0x100;
    uVar11 = (uint)((ulong)lVar2 >> 0x10);
    uVar6 = (uint)CARRY2((uint)lVar4,(uint)lVar2);
    uVar7 = uVar23 + uVar11;
    uVar16 = uVar7 + uVar6;
    uVar17 = (uint)CARRY2((uint)((ulong)lVar1 >> 0x10),
                          (uint)CARRY2((uint)bVar8 * (uint)bVar5,(uint)lVar1)) +
             (uint)(CARRY2(uVar23,uVar11) || CARRY2(uVar7,uVar6));
    lVar1 = (ulong)bVar8 * 0x100 * (ulong)uVar22;
    uVar12 = (uint)((ulong)lVar1 >> 0x10);
    uVar6 = (uint)lVar1;
    uVar24 = uVar16 + uVar6;
    uVar6 = (uint)CARRY2(uVar16,uVar6);
    uVar23 = uVar17 + uVar12;
    uVar18 = uVar23 + uVar6;
    uVar13 = (uint)((ulong)in_BX * (ulong)unaff_SI >> 0x10);
    uVar11 = (uint)((ulong)in_BX * (ulong)unaff_SI);
    uVar7 = (uint)CARRY2(uVar24,uVar11);
    uVar16 = uVar18 + uVar13;
    uVar19 = uVar16 + uVar7;
    lVar1 = (ulong)uVar21 * (ulong)bVar5 * 0x100;
    uVar14 = (uint)((ulong)lVar1 >> 0x10);
    uVar11 = (uint)CARRY2(uVar24 + uVar11,(uint)lVar1);
    uVar24 = uVar19 + uVar14;
    uVar20 = uVar24 + uVar11;
    uVar13 = (uint)(CARRY2(uVar17,uVar12) || CARRY2(uVar23,uVar6)) +
             (uint)(CARRY2(uVar18,uVar13) || CARRY2(uVar16,uVar7)) +
             (uint)(CARRY2(uVar19,uVar14) || CARRY2(uVar24,uVar11));
    uVar24 = (uint)((ulong)in_BX * (ulong)uVar22 >> 0x10);
    uVar6 = (uint)((ulong)in_BX * (ulong)uVar22);
    uVar23 = uVar20 + uVar6;
    uVar6 = (uint)CARRY2(uVar20,uVar6);
    uVar11 = uVar13 + uVar24;
    uVar14 = uVar11 + uVar6;
    uVar12 = (uint)((ulong)uVar21 * (ulong)unaff_SI >> 0x10);
    uVar7 = (uint)((ulong)uVar21 * (ulong)unaff_SI);
    uVar16 = uVar23 + uVar7;
    uVar7 = (uint)CARRY2(uVar23,uVar7);
    uVar23 = uVar14 + uVar12;
    uVar3 = (ulong)uVar21 * (ulong)uVar22 +
            CONCAT22((uint)(CARRY2(uVar13,uVar24) || CARRY2(uVar11,uVar6)) +
                     (uint)(CARRY2(uVar14,uVar12) || CARRY2(uVar23,uVar7)),uVar23 + uVar7);
  }
  if (-1 < (long)uVar3) {
    bVar25 = (int)uVar16 < 0;
    uVar16 = uVar16 << 1;
    uVar3 = CONCAT22((int)(uVar3 >> 0x10) << 1 | (uint)((uVar3 & 0x8000) != 0),
                     (int)((uVar3 & 0xffff) << 1 | (ulong)bVar25));
    iVar10 = iVar10 + -1;
  }
  uVar6 = (uint)(uVar3 >> 0x10);
  uVar22 = iVar10 + 0x7f7f;
  uVar21 = (uint)CARRY2((uint)uVar3,(uint)(0xff7f < uVar16));
  iVar15 = uVar6 + uVar21;
  if (CARRY2(uVar6,uVar21)) {
    iVar15 = (int)(CONCAT12(1,iVar15) >> 1);
    uVar22 = iVar10 + 0x7f80;
  }
  if ((uVar22 & 0x4000) != 0) {
    return 0;
  }
  return CONCAT22(CONCAT11((byte)((uint)iVar15 >> 8) ^ (byte)(uVar22 + 1 >> 8),(char)iVar15),
                  CONCAT11((char)(uVar16 + 0x80 >> 8),(char)(uVar22 + 1)));
}


