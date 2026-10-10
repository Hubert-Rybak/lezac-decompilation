/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x184ce FUN_184a_002e
 * ev_4f46e776a07c6dfd310b95b19489a3fcb6e1fe1694aec70274a861f825766e68
 */

/* WARNING: Removing unreachable block (ram,0x00018514) */

void __cdecl16near FUN_184a_002e(void)

{
  byte bVar1;
  byte extraout_AH;
  undefined2 unaff_DS;
  
  bVar1 = FUN_184a_060b();
  if ((bVar1 != 7) && (3 < bVar1)) {
    FUN_184a_0097();
  }
  FUN_184a_00e5();
  FUN_184a_060b();
  *(byte *)0x7f6c = extraout_AH & 0x7f;
  *(byte *)0x7f62 = extraout_AH & 0x7f;
  *(undefined1 *)0x7f5d = 0;
  *(undefined1 *)0x7f6d = 0;
  *(undefined1 *)0x7f6e = 0;
  *(undefined1 *)0x7f5c = 1;
  do {
  } while( true );
}


