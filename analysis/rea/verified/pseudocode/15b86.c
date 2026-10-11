/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x15b86 FUN_1000_5b86
 * ev_18f044a126a55ba191229bbfb11bcc20bc0a0b7781d28cdaa2838297aa895f2a
 */

void FUN_1000_5b86(int param_1)

{
  uint uVar1;
  undefined2 unaff_SS;
  
  FUN_1920_04df();
  uVar1 = (int)*(uint *)(param_1 + -0xc) >> 0xf;
  if ((int)((*(uint *)(param_1 + -0xc) ^ uVar1) - uVar1) < 0x2b) {
    *(undefined2 *)(param_1 + -0xc) = 0;
  }
  else if (*(int *)(param_1 + -0xc) < 0) {
    *(int *)(param_1 + -0xc) = *(int *)(param_1 + -0xc) + 0x2a;
  }
  else {
    *(int *)(param_1 + -0xc) = *(int *)(param_1 + -0xc) + -0x2a;
  }
  return;
}


