/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1846b FUN_183f_007b
 * ev_c37dd1a6ff12904dc7dc088c0fb705bd3a4328bdfb3b446fbdbac37b11756511
 */

void __stdcall16far FUN_183f_007b(undefined2 *param_1)

{
  code *pcVar1;
  undefined2 in_BX;
  undefined2 unaff_ES;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  *param_1 = in_BX;
  ((undefined2 *)param_1)[1] = unaff_ES;
  return;
}


