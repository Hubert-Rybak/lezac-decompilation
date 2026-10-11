/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19054 FUN_18ac_0594
 * ev_6427106e15c60c830b82b4f9390a704cad14ed24e0b0cbfe88bdde014578d333
 */

void __stdcall16far FUN_18ac_0594(int param_1)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined1 *puVar4;
  undefined1 *puVar5;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  if (*(char *)0xc496 != '\0') {
    if (param_1 < (int)(*(byte *)0xc496 - 1)) {
      puVar5 = (undefined1 *)(*(int *)0xc1fc + param_1 * 8);
      puVar4 = puVar5 + 8;
      for (iVar3 = (((uint)*(byte *)0xc496 - param_1) + -1) * 8; iVar3 != 0; iVar3 = iVar3 + -1) {
        puVar2 = puVar5;
        puVar5 = puVar5 + 1;
        puVar1 = puVar4;
        puVar4 = puVar4 + 1;
        *puVar2 = *puVar1;
      }
    }
    *(char *)0xc496 = *(char *)0xc496 + -1;
  }
  return;
}


