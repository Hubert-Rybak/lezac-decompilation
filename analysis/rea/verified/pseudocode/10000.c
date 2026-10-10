/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x10000 FUN_1000_0000
 * ev_11efe2ec60a5bbcae17774fa6e64390e587b6572ad0c7fe05538e60d0cb61449
 */

void FUN_1000_0000(uint param_1,int *param_2,byte *param_3,undefined2 param_4)

{
  uint local_4;
  
  FUN_1920_04df();
  if (param_1 != 0) {
    local_4 = 1;
    while( true ) {
      if ((local_4 & 1) == 1) {
        *param_2 = *param_2 + (uint)*param_3;
      }
      else {
        *param_2 = *param_2 - (uint)*param_3;
      }
      param_3 = param_3 + 1;
      if (local_4 == param_1) break;
      local_4 = local_4 + 1;
    }
  }
  return;
}


