/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a62f FUN_1920_142f
 * ev_5ff9e5583e4f7415cd6600bf6669470ec00003fcbd80298673864f702bda852a
 */

void __cdecl16far FUN_1920_142f(void)

{
  code *pcVar1;
  undefined2 in_CX;
  undefined2 extraout_DX;
  undefined2 unaff_DS;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  *(undefined2 *)0x1afe = in_CX;
  *(undefined2 *)0x1b00 = extraout_DX;
  return;
}


