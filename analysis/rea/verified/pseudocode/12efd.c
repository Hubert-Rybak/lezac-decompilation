/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x12efd FUN_1000_2efd
 * ev_fef85fe7c04b1914a5a5727984cc3b5b76b2b3c1b5a4730ef15d377d4206fe34
 */

void __cdecl16near FUN_1000_2efd(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  FUN_1920_15ca(0x2ef2,0x1920,0x7eda,unaff_DS);
  FUN_1920_15f8(1,0x7eda,unaff_DS);
  iVar2 = FUN_1920_04a2();
  if (iVar2 != 0) {
    FUN_1000_00a3(1);
  }
  *(undefined1 *)0x1bac = 100;
  *(undefined1 *)0x1bd2 = 100;
  *(undefined1 *)0x79c7 = 0;
  *(undefined1 *)0x79e6 = 1;
  *(undefined1 *)0x79e7 = 1;
  *(undefined1 *)0xc49c = 1;
  if (*(char *)0x79b8 == '\x01') {
    *(undefined1 *)0x79e7 = 0;
  }
  *(undefined1 *)0x79b7 = 1;
  *(undefined1 *)0x79ea = 2;
  *(undefined1 *)0x79eb = 2;
  *(undefined1 *)0x1b6c = 200;
  *(undefined1 *)0x1b6d = 0x14;
  *(undefined1 *)0x1b6e = 6;
  *(undefined1 *)0x1b6f = 0;
  uVar1 = *(undefined2 *)0x1b6e;
  *(undefined2 *)0x1b70 = *(undefined2 *)0x1b6c;
  *(undefined2 *)0x1b72 = uVar1;
  FUN_1000_2e8d(&stack0xfffe,0x785a,unaff_DS);
  FUN_1000_2e8d(&stack0xfffe,0x7888,unaff_DS);
  return;
}


