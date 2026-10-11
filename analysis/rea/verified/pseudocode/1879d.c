/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1879d FUN_184a_02fd
 * ev_0114a4d4f644a6d7aa33f6a6016278ecafb625f1b3de4783bbd812ccff77217c
 */

undefined1 __cdecl16far FUN_184a_02fd(void)

{
  code *pcVar1;
  undefined2 unaff_DS;
  bool bVar2;
  
  bVar2 = *(char *)0x7f6d == '\0';
  if (bVar2) {
    pcVar1 = (code *)swi(0x16);
    (*pcVar1)();
    if (bVar2) {
      return 0;
    }
  }
  return 1;
}


