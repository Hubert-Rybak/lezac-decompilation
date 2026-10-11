/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1568a FUN_1000_568a
 * ev_e0a470979b531334de2f1053766c892832cc77216e4dd917ee49d78c810a9c4a
 */

void __cdecl16near FUN_1000_568a(void)

{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  undefined1 *puVar4;
  undefined2 unaff_DS;
  
  uVar2 = *(uint *)0x2074;
  if (uVar2 < 0x65) {
    puVar4 = (undefined1 *)(*(int *)0x2072 + -0x31c0);
    for (; uVar2 != 0; uVar2 = uVar2 - 1) {
      puVar1 = puVar4;
      puVar4 = puVar4 + 1;
      *puVar1 = 0xe;
    }
    for (iVar3 = 100 - *(int *)0x2074; iVar3 != 0; iVar3 = iVar3 + -1) {
      puVar1 = puVar4;
      puVar4 = puVar4 + 1;
      *puVar1 = 1;
    }
  }
  return;
}


