/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x15a75 FUN_1000_5a75
 * ev_f1d1c0cdbc90f3d1c1c3adfcbb2624dacb4c69ee794d802ecd631815e3d268d4
 */

void FUN_1000_5a75(int param_1,byte param_2)

{
  int iVar1;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  iVar1 = (uint)*(byte *)((int)*(undefined4 *)(param_1 + 4) + 1) * 8;
  *(undefined2 *)(iVar1 + -0x3ddc) = *(undefined2 *)((uint)param_2 * 4 + -0x3cdc);
  *(undefined1 *)(iVar1 + -0x3dde) = *(undefined1 *)((uint)param_2 * 4 + -0x3cde);
  *(undefined1 *)(iVar1 + -0x3ddd) = *(undefined1 *)((uint)param_2 * 4 + -0x3cdd);
  *(char *)((int)*(undefined4 *)(param_1 + 4) + 0x14) = '\x10' - *(char *)(iVar1 + -0x3ddd);
  return;
}


