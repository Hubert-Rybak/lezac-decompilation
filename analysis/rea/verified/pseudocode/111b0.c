/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x111b0 FUN_1000_11b0
 * ev_3a93a746ed2e048c738b663a6af5e91e7916a4bc95cf19eae4d3b9b73439d44e
 */

void FUN_1000_11b0(undefined4 param_1)

{
  int iVar1;
  byte bVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 local_8;
  undefined2 local_4;
  
  FUN_1920_04df();
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  local_4 = *(int *)(iVar3 + 2);
  local_8 = 0;
  do {
    bVar2 = 10;
    FUN_1920_0945();
    *(int *)(iVar3 + local_8 * 2 + 0x18) = (uint)bVar2 * 0x40 + *(int *)0x2070;
    iVar1 = FUN_1920_0945();
    local_8 = local_8 + 1;
    if (iVar1 == 0 && local_4 == 0) break;
  } while (local_8 != 8);
  *(undefined1 *)(iVar3 + 0x2c) = 1;
  return;
}


