/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1999c FUN_1920_079c
 * ev_f7acdcffabed15aef38e55df505ca13a98d78cda8ffaa5febe944990e80da5ff
 */

undefined1 __cdecl16near FUN_1920_079c(undefined4 param_1)

{
  int iVar1;
  int in_DX;
  int in_BX;
  int unaff_DI;
  undefined2 unaff_ES;
  
  if (in_BX == in_DX) {
    unaff_ES = (undefined2)((ulong)param_1 >> 0x10);
    unaff_DI = (int)param_1;
    iVar1 = FUN_1920_07eb();
    if (in_BX == iVar1) {
      return 0x1a;
    }
  }
  return *(undefined1 *)(in_BX + unaff_DI);
}


