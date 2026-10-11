/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19a1c FUN_1920_081c
 * ev_7a1a71520c23e58f1f86b6712b9f2170280fe064079ba504415984ae3dc36e32
 */

void __stdcall16far FUN_1920_081c(undefined4 param_1)

{
  char cVar1;
  int in_BX;
  undefined1 in_ZF;
  
  FUN_1920_0778();
  if ((bool)in_ZF) {
    do {
      cVar1 = FUN_1920_079c();
      if (cVar1 == '\x1a') goto LAB_1920_083d;
      in_BX = in_BX + 1;
    } while (cVar1 != '\r');
    cVar1 = FUN_1920_079c();
    if (cVar1 == '\n') {
      in_BX = in_BX + 1;
    }
  }
LAB_1920_083d:
  *(int *)((int)param_1 + 8) = in_BX;
  FUN_1920_087b();
  return;
}


