/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1458d FUN_1000_458d
 * ev_c05156b71259cbbc3c0f8a7a31887b323c936bff398b916e5914b68ce85f7ac2
 */

void FUN_1000_458d(int param_1,uint param_2)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  undefined1 *puVar5;
  undefined1 *puVar6;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  iVar4 = (int)param_2 >> 0xf;
  if ((iVar4 < 0) || ((-1 < (int)param_2 && (param_2 < *(uint *)0x207e)))) {
    puVar6 = (undefined1 *)(*(int *)0x207c + (param_2 - 1) * 0xb);
    puVar5 = puVar6 + 0xb;
    for (iVar3 = (*(int *)0x207e - param_2) * 0xb; iVar3 != 0; iVar3 = iVar3 + -1) {
      puVar2 = puVar6;
      puVar6 = puVar6 + 1;
      puVar1 = puVar5;
      puVar5 = puVar5 + 1;
      *puVar2 = *puVar1;
    }
  }
  if (((int)param_2 < 0 && iVar4 < 1) || ((iVar4 < 1 && (param_2 <= *(uint *)(param_1 + -2))))) {
    *(int *)(param_1 + -2) = *(int *)(param_1 + -2) + -1;
  }
  *(int *)0x207e = *(int *)0x207e + -1;
  return;
}


