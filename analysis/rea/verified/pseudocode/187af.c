/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x187af FUN_184a_030f
 * ev_1a3a72741b7c21c99230db7ff2d17b51aa25da8c009256fd086457949bc7f8a9
 */

void __cdecl16far FUN_184a_030f(void)

{
  code *pcVar1;
  char cVar2;
  undefined1 extraout_AH;
  undefined2 unaff_DS;
  
  cVar2 = *(char *)0x7f6d;
  *(undefined1 *)0x7f6d = 0;
  if (cVar2 == '\0') {
    pcVar1 = (code *)swi(0x16);
    cVar2 = (*pcVar1)();
    if (cVar2 == '\0') {
      *(undefined1 *)0x7f6d = extraout_AH;
    }
  }
  FUN_184a_0143();
  return;
}


