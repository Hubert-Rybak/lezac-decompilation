/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1313d FUN_1000_313d
 * ev_e9f61d597fbf4ef191c8e248b6d4295c2ff79de3a395b050759f1b30b58f3230
 */

void FUN_1000_313d(int param_1,uint param_2)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  uVar1 = 0x1000;
  FUN_1920_04df();
  for (; param_1 != 0; param_1 = param_1 + -1) {
    *(int *)0x2074 = (param_2 % 10) * 0x40 + *(int *)0x2070;
    FUN_1000_1156(uVar1);
    *(int *)0x2072 = *(int *)0x2072 + -9;
    param_2 = param_2 / 10;
  }
  return;
}


