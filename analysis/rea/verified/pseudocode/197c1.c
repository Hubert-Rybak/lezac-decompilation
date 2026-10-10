/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x197c1 FUN_1920_05c1
 * ev_59783e30d8ac04c8a9d7629f2f09c5c97bd5911cf2400ec581dfc576ce731341
 */

void __stdcall16far FUN_1920_05c1(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  if (*(int *)(iVar1 + 2) != -0x284f) {
    if (*(int *)(iVar1 + 2) != -0x284e) {
      *(undefined2 *)0x1afc = 0x67;
      return;
    }
    FUN_1920_05fd();
  }
  FUN_1920_05fd();
  *(undefined2 *)(iVar1 + 2) = 0xd7b0;
  return;
}


