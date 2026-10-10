/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19a7b FUN_1920_087b
 * ev_03a470ef537528a810bde017dd49f9e6e4e08df04a358f85071143df0faa9556
 */

void __cdecl16near FUN_1920_087b(void)

{
  int iVar1;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  if (*(int *)(unaff_DI + 0x1a) == 0) {
    return;
  }
  if (*(int *)0x1afc == 0) {
    iVar1 = (*(code *)*(undefined2 *)(unaff_DI + 0x18))(0x1920);
    if (iVar1 != 0) {
      *(int *)0x1afc = iVar1;
    }
  }
  return;
}


