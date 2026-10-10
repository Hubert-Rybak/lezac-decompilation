/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1452a FUN_1000_452a
 * ev_5e5a45070257ab92731b335a250f9fbd6162ab19dbfd5d5baccecd4be2725068
 */

void FUN_1000_452a(undefined2 param_1,uint param_2)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  undefined1 *puVar5;
  undefined1 *puVar6;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  if (((int)param_2 >> 0xf < 0) || ((-1 < (int)param_2 && (param_2 < *(uint *)0x2076)))) {
    puVar6 = (undefined1 *)(*(int *)0x207c + (param_2 - 1) * 0xb);
    puVar5 = puVar6 + 0xb;
    for (iVar3 = (*(int *)0x2076 - param_2) * 0xb; iVar3 != 0; iVar3 = iVar3 + -1) {
      puVar2 = puVar6;
      puVar6 = puVar6 + 1;
      puVar1 = puVar5;
      puVar5 = puVar5 + 1;
      *puVar2 = *puVar1;
    }
    iVar3 = *(int *)0x2076 - param_2;
    iVar4 = param_2 - 1;
    do {
      *(undefined1 *)(iVar4 + 0x78d6) = *(undefined1 *)(iVar4 + 0x78d7);
      iVar4 = iVar4 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  *(int *)0x2076 = *(int *)0x2076 + -1;
  return;
}


