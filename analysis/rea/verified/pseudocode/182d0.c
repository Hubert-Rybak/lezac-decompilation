/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x182d0 FUN_182d_0000
 * ev_d223032ba111d1431e9e5bd1dee5164dbab39e9df314ffe479009f57f3e6b9d5
 */

void __stdcall16far FUN_182d_0000(uint param_1,int *param_2,undefined2 param_3,int *param_4)

{
  byte bVar1;
  undefined1 uVar2;
  undefined1 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  byte bVar8;
  uint uVar9;
  int iVar10;
  uint local_16;
  uint local_14;
  int local_12;
  
  FUN_1920_04df();
  local_12 = 0;
  local_14 = 0;
  iVar4 = *param_4;
  iVar5 = ((int *)param_4)[1];
  iVar6 = *param_2;
  iVar7 = ((int *)param_2)[1];
  do {
    bVar1 = *(byte *)(iVar4 + local_12);
    uVar2 = *(undefined1 *)(iVar4 + local_12 + 1);
    uVar3 = *(undefined1 *)(iVar4 + local_12 + 2);
    local_12 = local_12 + 3;
    bVar8 = (bVar1 >> 4) + 1;
    uVar9 = bVar8 + local_14;
    if (local_14 <= uVar9) {
      for (local_16 = local_14; *(undefined1 *)(iVar6 + local_16) = uVar2, local_16 != uVar9;
          local_16 = local_16 + 1) {
      }
    }
    local_14 = local_14 + bVar8;
    if (local_14 < param_1) {
      iVar10 = (uint)bVar1 % 0x10 + 1;
      uVar9 = iVar10 + local_14;
      local_16 = local_14;
      if (local_14 <= uVar9) {
        for (; *(undefined1 *)(iVar6 + local_16) = uVar3, local_16 != uVar9; local_16 = local_16 + 1
            ) {
        }
      }
      local_14 = local_14 + iVar10;
    }
  } while (local_14 < param_1);
  return;
}


