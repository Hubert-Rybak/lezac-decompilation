/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x13f27 FUN_1000_3f27
 * ev_41aca508981d010caf2271f7186ec853f54c3357977aa922bb4cdad34d1cb8e9
 */

void __cdecl16near FUN_1000_3f27(void)

{
  char cVar1;
  int iVar2;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  iVar2 = FUN_1920_13a8(*(undefined2 *)0xc204);
  iVar2 = iVar2 + *(int *)0xc204;
  if (*(char *)((int)*(undefined4 *)0xc1e0 + iVar2) == '\0') {
    cVar1 = FUN_1920_13a8(9);
    *(char *)0x79a3 = cVar1 + 'g';
    if (*(char *)0x79a3 == *(char *)0x79b4) {
      *(char *)0x79a3 = *(char *)0x79a3 + '\x01';
    }
    *(undefined1 *)((int)*(undefined4 *)0xc1e0 + iVar2) = *(undefined1 *)0x79a3;
    *(undefined2 *)((int)*(undefined4 *)0x6612 + iVar2 * 2) = *(undefined2 *)0x78c4;
    *(int *)0x78c4 = *(int *)0x78c4 + 1;
    FUN_1000_370e(0x79a3,unaff_DS,0,0,iVar2);
  }
  return;
}


