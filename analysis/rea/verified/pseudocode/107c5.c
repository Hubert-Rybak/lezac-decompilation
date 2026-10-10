/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x107c5 FUN_1000_07c5
 * ev_0dd0ea530e49d7d580f0ccfeecbfa00bbacb12f13dadc39535aa9233fd0d311d
 */

void FUN_1000_07c5(undefined1 param_1,int param_2,int param_3,int param_4,int param_5)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined1 *puVar4;
  
  FUN_1920_04df();
  puVar2 = (undefined1 *)(param_4 * 0x140 + param_5);
  iVar3 = param_3;
  puVar4 = puVar2;
  do {
    for (; iVar3 != 0; iVar3 = iVar3 + -1) {
      puVar1 = puVar2;
      puVar2 = puVar2 + 1;
      *puVar1 = param_1;
    }
    puVar2 = puVar4 + 0x140;
    param_2 = param_2 + -1;
    iVar3 = param_3;
    puVar4 = puVar2;
  } while (param_2 != 0);
  return;
}


