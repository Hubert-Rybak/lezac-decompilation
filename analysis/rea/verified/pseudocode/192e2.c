/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x192e2 FUN_1920_00e2
 * ev_a05edfbfc63198f43b9ff0451f0e7f3af017fadb26f059539a60a079bd8034b0
 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Restarted to delay deadcode elimination for space: ram */

void __cdecl16far FUN_1920_00e2(void)

{
  code *pcVar1;
  undefined2 in_AX;
  int iVar2;
  int iVar3;
  char *pcVar4;
  int in_stack_00000000;
  int in_stack_00000002;
  
  DAT_1aa2_1af2 = in_AX;
  iVar2 = DAT_1aa2_1ad0;
  if (in_stack_00000000 != 0 || in_stack_00000002 != 0) {
    for (; (iVar3 = in_stack_00000002, iVar2 != 0 &&
           (iVar3 = iVar2, in_stack_00000002 != *(int *)0x10)); iVar2 = *(int *)0x14) {
    }
    in_stack_00000002 = (iVar3 - DAT_1aa2_1af8) + -0x10;
  }
  DAT_1aa2_1af4 = in_stack_00000000;
  DAT_1aa2_1af6 = in_stack_00000002;
  pcVar4 = (char *)_DAT_1aa2_1aee;
  if ((int)((ulong)_DAT_1aa2_1aee >> 0x10) == 0 && pcVar4 == (char *)0x0) {
    FUN_1920_05c1();
    FUN_1920_05c1();
    iVar2 = 0x13;
    do {
      pcVar1 = (code *)swi(0x21);
      (*pcVar1)();
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    if (DAT_1aa2_1af4 != 0 || DAT_1aa2_1af6 != 0) {
      FUN_1920_01a5();
      FUN_1920_01b3();
      FUN_1920_01a5();
      FUN_1920_01cd();
      FUN_1920_01e7();
      FUN_1920_01cd();
      pcVar4 = (char *)0x215;
      FUN_1920_01a5();
    }
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    for (; *pcVar4 != '\0'; pcVar4 = pcVar4 + 1) {
      FUN_1920_01e7();
    }
    return;
  }
  _DAT_1aa2_1aee = (char *)0x0;
  DAT_1aa2_1afc = 0;
  return;
}


