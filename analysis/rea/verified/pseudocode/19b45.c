/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19b45 FUN_1920_0945
 * ev_363668f80bb4216458378ecc5f284288c417be758c7361b330d7171d9d80fe9d
 */

uint __cdecl16far FUN_1920_0945(void)

{
  uint uVar1;
  ulong uVar2;
  uint in_AX;
  uint in_CX;
  uint uVar3;
  uint uVar4;
  uint in_DX;
  uint in_BX;
  uint uVar5;
  uint uVar6;
  int iVar7;
  bool bVar8;
  bool bVar9;
  bool bVar10;
  bool bVar11;
  
  if (in_CX == 0 && in_BX == 0) {
    uVar3 = FUN_1920_00e2();
    return uVar3;
  }
  bVar10 = (int)in_BX < 0;
  if (bVar10) {
    uVar3 = ~in_CX;
    in_CX = uVar3 + 1;
    in_BX = ~in_BX + (uint)(0xfffe < uVar3);
  }
  bVar11 = (int)in_DX < 0;
  if (bVar11) {
    uVar3 = ~in_AX;
    in_AX = uVar3 + 1;
    in_DX = ~in_DX + (uint)(0xfffe < uVar3);
  }
  uVar3 = 0;
  bVar8 = false;
  uVar5 = 0;
  iVar7 = 0x21;
  do {
    uVar2 = (ulong)CONCAT12(bVar8,uVar3) << 1;
    uVar4 = (uint)uVar2 | (uint)bVar8;
    uVar6 = uVar5 << 1 | (uint)((uVar2 & 0x10000) != 0);
    uVar3 = uVar4 - in_CX;
    uVar4 = (uint)(uVar4 < in_CX);
    uVar1 = uVar6 - in_BX;
    bVar8 = uVar6 < in_BX || uVar1 < uVar4;
    uVar5 = uVar1 - uVar4;
    if (uVar6 < in_BX || uVar1 < uVar4) {
      bVar9 = CARRY2(uVar3,in_CX);
      uVar3 = uVar3 + in_CX;
      bVar8 = CARRY2(uVar5,in_BX) || CARRY2(uVar5 + in_BX,(uint)bVar9);
      uVar5 = uVar5 + in_BX + (uint)bVar9;
    }
    uVar2 = (ulong)CONCAT12(!bVar8,in_AX) << 1;
    in_AX = (uint)uVar2 | (uint)!bVar8;
    bVar8 = (uVar2 & 0x10000) != 0;
    uVar2 = (ulong)CONCAT12(bVar8,in_DX) << 1;
    in_DX = (uint)uVar2 | (uint)bVar8;
    bVar8 = (uVar2 & 0x10000) != 0;
    iVar7 = iVar7 + -1;
  } while (iVar7 != 0);
  if (bVar11) {
    if (bVar10) {
      return in_AX;
    }
  }
  else if (!bVar10) {
    return in_AX;
  }
  return ~in_AX + 1;
}


