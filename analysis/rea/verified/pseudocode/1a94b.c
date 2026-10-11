/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a94b FUN_1920_174b
 * ev_54118a8991beee8c77663b78afed2d8d36dc47e4d5e6972eef8305d9adb5ad60
 */

void __stdcall16far FUN_1920_174b(uint param_1,int param_2,undefined4 param_3)

{
  code *pcVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  bool bVar3;
  undefined1 in_ZF;
  
  uVar2 = (undefined2)((ulong)param_3 >> 0x10);
  FUN_1920_169e();
  if ((bool)in_ZF) {
    bVar3 = CARRY2(param_2 * *(int *)((int)param_3 + 4),
                   (uint)((ulong)param_1 * (ulong)*(uint *)((int)param_3 + 4) >> 0x10));
    pcVar1 = (code *)swi(0x21);
    uVar2 = (*pcVar1)();
    if (bVar3) {
      *(undefined2 *)0x1afc = uVar2;
    }
  }
  return;
}


