/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19fad FUN_1920_0dad
 * ev_91ac6f6fe797fa1212d5117b1551e6d4c2a2a03ecb67fc34e6c3f7c6608e9d6e
 */

undefined4 __cdecl16near FUN_1920_0dad(void)

{
  ulong uVar1;
  byte bVar2;
  undefined1 uVar3;
  char cVar4;
  undefined2 in_AX;
  byte bVar5;
  undefined2 in_CX;
  byte bVar7;
  uint uVar6;
  int iVar8;
  uint in_DX;
  uint uVar9;
  uint in_BX;
  uint unaff_BP;
  uint uVar10;
  uint unaff_SI;
  uint unaff_DI;
  uint uVar11;
  bool bVar12;
  int in_stack_00000000;
  
  bVar2 = (byte)in_AX;
  if (bVar2 == 0) {
    return 0;
  }
  uVar11 = unaff_DI | 0x8000;
  uVar10 = in_DX | 0x8000;
  iVar8 = CONCAT11((char)((uint)in_AX >> 8),2);
  uVar9 = 1;
  do {
    bVar12 = uVar10 < uVar11;
    bVar7 = (byte)((uint)in_CX >> 8);
    bVar5 = (byte)((uint)iVar8 >> 8);
    if ((uVar10 == uVar11) && (bVar12 = in_BX < unaff_SI, in_BX == unaff_SI)) {
      bVar12 = bVar5 < bVar7;
    }
    if (!bVar12) {
      iVar8 = CONCAT11(bVar5 - bVar7,(char)iVar8);
      bVar12 = in_BX < unaff_SI;
      uVar6 = in_BX - unaff_SI;
      in_BX = uVar6 - (bVar5 < bVar7);
      uVar6 = (uint)(bVar12 || uVar6 < (bVar5 < bVar7));
      bVar12 = uVar10 < uVar11 || uVar10 - uVar11 < uVar6;
      uVar10 = (uVar10 - uVar11) - uVar6;
    }
    while( true ) {
      uVar1 = (ulong)CONCAT12(bVar12,uVar9) << 1;
      uVar9 = (uint)uVar1 | (uint)bVar12;
      if ((uVar1 & 0x10000) != 0) {
        cVar4 = (char)iVar8 + -1;
        iVar8 = CONCAT11((char)((uint)iVar8 >> 8),cVar4);
        if (cVar4 < '\0') {
          uVar9 = ~(uVar9 << 6);
          uVar11 = ~CONCAT11((((byte)(in_DX >> 8) ^ (byte)(unaff_DI >> 8)) & 0x80) -
                             (bVar2 < (byte)in_CX),bVar2 - (byte)in_CX);
          uVar10 = unaff_BP ^ 0xffff;
          if (-1 < (int)uVar10) {
            uVar1 = (ulong)uVar9;
            uVar9 = (uint)(uVar1 << 1);
            bVar12 = (uVar1 << 1 & 0x10000) != 0;
            uVar1 = (ulong)CONCAT12(bVar12,uVar11) << 1;
            uVar11 = (uint)uVar1 | (uint)bVar12;
            uVar10 = (unaff_BP ^ 0xffff) << 1 | (uint)((uVar1 & 0x10000) != 0);
            in_stack_00000000 = in_stack_00000000 + -1;
          }
          uVar6 = in_stack_00000000 + 0x8080;
          uVar11 = (uint)CARRY2(uVar11,(uint)(0xff7f < uVar9));
          iVar8 = uVar10 + uVar11;
          if (CARRY2(uVar10,uVar11)) {
            iVar8 = (int)(CONCAT12(1,iVar8) >> 1);
            uVar6 = in_stack_00000000 + 0x8081;
          }
          if ((uVar6 & 0x4000) != 0) {
            return 0;
          }
          return CONCAT22(CONCAT11((byte)((uint)iVar8 >> 8) ^ (byte)(uVar6 + 1 >> 8),(char)iVar8),
                          CONCAT11((char)(uVar9 + 0x80 >> 8),(char)(uVar6 + 1)));
        }
        uVar9 = 1;
        if (cVar4 == '\0') {
          uVar9 = 0x40;
        }
      }
      bVar12 = iVar8 < 0;
      uVar3 = (undefined1)iVar8;
      bVar5 = (char)((uint)iVar8 >> 8) * '\x02';
      iVar8 = CONCAT11(bVar5,uVar3);
      uVar1 = (ulong)CONCAT12(bVar12,in_BX) << 1;
      in_BX = (uint)uVar1 | (uint)bVar12;
      bVar12 = (uVar1 & 0x10000) != 0;
      uVar1 = (ulong)CONCAT12(bVar12,uVar10) << 1;
      uVar10 = (uint)uVar1 | (uint)bVar12;
      if ((uVar1 & 0x10000) == 0) break;
      iVar8 = CONCAT11(bVar5 - bVar7,uVar3);
      bVar12 = in_BX < unaff_SI;
      uVar6 = in_BX - unaff_SI;
      in_BX = uVar6 - (bVar5 < bVar7);
      uVar10 = (uVar10 - uVar11) - (uint)(bVar12 || uVar6 < (bVar5 < bVar7));
      bVar12 = false;
    }
  } while( true );
}


