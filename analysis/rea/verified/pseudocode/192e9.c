/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x192e9 FUN_1920_00e9
 * ev_7f1c6a96010a351fa6b7781a0d06e833854322b724fabaf2eb44667bd08d9107
 */

/* WARNING: Removing unreachable block (ram,0x000192fc) */
/* WARNING: Removing unreachable block (ram,0x000192ff) */
/* WARNING: Removing unreachable block (ram,0x00019303) */
/* WARNING: Removing unreachable block (ram,0x00019312) */
/* WARNING: Removing unreachable block (ram,0x0001930c) */
/* WARNING: Removing unreachable block (ram,0x00019314) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_1920_00e9(void)

{
  code *pcVar1;
  undefined2 in_AX;
  int iVar2;
  char *pcVar3;
  undefined2 uVar4;
  
  uVar4 = 0x1aa2;
  DAT_1aa2_1af2 = in_AX;
  DAT_1aa2_1af4 = 0;
  DAT_1aa2_1af6 = 0;
  pcVar3 = (char *)_DAT_1aa2_1aee;
  if ((int)((ulong)_DAT_1aa2_1aee >> 0x10) == 0 && pcVar3 == (char *)0x0) {
    FUN_1920_05c1(0xc49e,0x1aa2);
    FUN_1920_05c1(0xc59e,0x1aa2);
    iVar2 = 0x13;
    do {
      pcVar1 = (code *)swi(0x21);
      (*pcVar1)();
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    if (*(int *)0x1af4 != 0 || *(int *)0x1af6 != 0) {
      FUN_1920_01a5();
      FUN_1920_01b3();
      FUN_1920_01a5();
      FUN_1920_01cd();
      FUN_1920_01e7();
      FUN_1920_01cd();
      pcVar3 = (char *)0x215;
      FUN_1920_01a5();
    }
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    for (; *pcVar3 != '\0'; pcVar3 = pcVar3 + 1) {
      FUN_1920_01e7();
    }
    return;
  }
  _DAT_1aa2_1aee = (char *)0x0;
  DAT_1aa2_1afc = 0;
  return;
}


