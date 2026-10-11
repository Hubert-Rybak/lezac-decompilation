/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x185e3 FUN_184a_0143
 * ev_38b4ca2a94d54f48237d5020ecc8d13e16d8e0e0b7ce79a0c3e25bb747f9b391
 */

void __cdecl16near FUN_184a_0143(void)

{
  code *pcVar1;
  undefined2 unaff_DS;
  undefined1 uVar2;
  
  if (*(char *)0x7f6e == '\0') {
    return;
  }
  *(undefined1 *)0x7f6e = 0;
  uVar2 = 0;
  while( true ) {
    pcVar1 = (code *)swi(0x16);
    (*pcVar1)();
    if ((bool)uVar2) break;
    pcVar1 = (code *)swi(0x16);
    (*pcVar1)();
  }
  FUN_184a_047e();
  FUN_184a_047e();
  FUN_184a_0477();
  pcVar1 = (code *)swi(0x23);
  (*pcVar1)();
  FUN_184a_0097();
  FUN_184a_00e5();
  *(undefined1 *)0x7f62 = *(undefined1 *)0x7f6c;
  return;
}


