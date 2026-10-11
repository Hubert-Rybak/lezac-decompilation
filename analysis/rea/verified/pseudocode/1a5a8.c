/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a5a8 FUN_1920_13a8
 * ev_0a50ef28a840920032a525518f279c4da98a9b0c83c559853086fa02607bef8d
 */

uint __stdcall16far FUN_1920_13a8(uint param_1)

{
  uint uVar1;
  uint extraout_DX;
  
  FUN_1920_13f7();
  uVar1 = 0;
  if (param_1 != 0) {
    uVar1 = extraout_DX % param_1;
  }
  return uVar1;
}


