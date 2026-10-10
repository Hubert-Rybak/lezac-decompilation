/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x199dc FUN_1920_07dc
 * ev_53449cfec95acd37e527df94db27bfacf52bc66881a233732276b0b27ccc5200
 */

undefined4 __cdecl16near FUN_1920_07dc(undefined4 param_1)

{
  undefined2 in_AX;
  int iVar1;
  int in_DX;
  int in_BX;
  int unaff_DI;
  int iVar2;
  undefined2 unaff_ES;
  undefined2 uVar3;
  
  *(undefined1 *)(in_BX + unaff_DI) = (char)in_AX;
  if (in_BX + 1 != in_DX) {
    return CONCAT22(in_DX,in_AX);
  }
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  *(int *)(iVar2 + 8) = in_BX + 1;
  iVar1 = (*(code *)*(undefined2 *)(iVar2 + 0x14))(0x1920,iVar2,uVar3,iVar2,uVar3);
  if (iVar1 != 0) {
    DAT_1aa2_1afc = iVar1;
  }
  return CONCAT22(*(undefined2 *)(iVar2 + 4),*(undefined2 *)(iVar2 + 10));
}


