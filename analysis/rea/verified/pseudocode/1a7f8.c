/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a7f8 FUN_1920_15f8
 * ev_df4f6d9ebd1df59c0aa656d3c60095bc2852e6326bc46fe5684752e1ea533dab
 */

void __stdcall16far FUN_1920_15f8(undefined2 param_1,undefined2 *param_2)

{
  code *pcVar1;
  undefined2 uVar2;
  undefined2 *puVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  bool bVar5;
  undefined2 in_stack_00000000;
  
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  puVar3 = (undefined2 *)param_2;
  if (puVar3[1] != -0x2850) {
    if (puVar3[1] != -0x284d) {
      *(undefined2 *)0x1afc = 0x66;
      return;
    }
    FUN_1920_1679(puVar3,uVar4);
  }
  uVar2 = 0;
  if (*(char *)(puVar3 + 0x18) != '\0') {
    bVar5 = false;
    pcVar1 = (code *)swi(0x21);
    uVar2 = (*pcVar1)();
    if (bVar5) {
      *(undefined2 *)0x1afc = uVar2;
      return;
    }
  }
  puVar3[1] = 0xd7b3;
  *param_2 = uVar2;
  puVar3[2] = param_1;
  return;
}


