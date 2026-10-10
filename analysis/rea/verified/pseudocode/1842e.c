/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1842e FUN_183f_003e
 * ev_08916c6c49fa2842cfbb6ec9d774a4c2cb99dbbd1487e5715c1d030d567db5ab
 */

void __stdcall16far FUN_183f_003e(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  FUN_183f_0058();
  return;
}


