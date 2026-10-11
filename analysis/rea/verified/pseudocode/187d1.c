/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x187d1 FUN_184a_0331
 * ev_5ffe77dec03e9ff151f134a8ca46744aff6e20730bda37b911fada14b24a7be3
 */

void __stdcall16far FUN_184a_0331(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  *(undefined2 *)(iVar1 + 2) = 0xd7b0;
  *(undefined2 *)(iVar1 + 4) = 0x80;
  *(int *)(iVar1 + 0xc) = iVar1 + 0x80;
  *(undefined2 *)(iVar1 + 0xe) = uVar2;
  *(undefined2 *)(iVar1 + 0x10) = 0x35c;
  *(undefined2 *)(iVar1 + 0x12) = 0x184a;
  *(undefined1 *)(iVar1 + 0x30) = 0;
  return;
}


