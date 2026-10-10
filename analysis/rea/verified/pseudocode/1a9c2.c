/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a9c2 FUN_1920_17c2
 * ev_adb628130fb1a434a8e424f3bd09af4b5625297e324ec37c18775fb171648d41
 */

undefined2 __cdecl16near FUN_1920_17c2(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  code *pcVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  if (*(int *)((int)param_3 + 2) == -0x284d) {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    pcVar1 = (code *)swi(0x21);
    uVar2 = (*pcVar1)();
    return uVar2;
  }
  *(undefined2 *)0x1afc = 0x67;
  return 0;
}


