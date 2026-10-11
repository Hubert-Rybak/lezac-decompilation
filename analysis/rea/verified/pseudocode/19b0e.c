/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19b0e FUN_1920_090e
 * ev_f35bee13c7944191e2e65123fda7f1a5b62e16df872f044d06d2bf42cd18942a
 */

void __stdcall16far FUN_1920_090e(int param_1,undefined1 *param_2,undefined1 *param_3)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  
  puVar3 = (undefined1 *)param_3;
  puVar4 = (undefined1 *)param_2;
  for (; param_1 != 0; param_1 = param_1 + -1) {
    puVar2 = puVar4;
    puVar4 = puVar4 + 1;
    puVar1 = puVar3;
    puVar3 = puVar3 + 1;
    *puVar2 = *puVar1;
  }
  return;
}


