/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x10838 FUN_1000_0838
 * ev_4e4bbc29b55fc4be5fb598406dc7e9433d62f03a5c48b340672625774eed6171
 */

void FUN_1000_0838(undefined2 param_1)

{
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  *(undefined2 *)0xc1ea = param_1;
  *(int *)0xc206 = *(int *)0xc1ea * -8 + 0x140;
  *(int *)0xc1ec = *(int *)0xc1ea << 3;
  *(int *)0x2094 = (*(int *)0xc204 - *(int *)0xc1ea) * 8;
  *(int *)0x78bc = (*(int *)0xc1ea - 2U >> 1) << 3;
  *(uint *)0x78be = *(uint *)0x78bc >> 3;
  *(uint *)0xc1f4 = (*(int *)0xc1ea + -1) * -8 + 0x140U >> 1;
  *(int *)0xc1f6 = *(int *)0xc1f8 - *(int *)0xc1ec;
  return;
}


