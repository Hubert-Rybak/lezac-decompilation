/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1414a FUN_1000_414a
 * ev_dd393b24dcb33ba37680d659cf47d45250c4bf88485b70ef33b2c1286ac91532
 */

void FUN_1000_414a(byte param_1,int param_2)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  uVar1 = 0x4155;
  FUN_1920_04df();
  if (1 < param_1) {
    *(undefined2 *)0x209a = 2;
    *(undefined2 *)0x209c = 1;
  }
  if (param_1 == 1) {
    uVar1 = 0x805;
    FUN_1000_3fa6(&stack0xfffe,0x50,1,0x86,0,0x3400,0x89,0,0x2a00,0,0,0,param_2);
    *(undefined2 *)0x2074 = 0xea74;
    *(undefined1 *)0x799f = 4;
  }
  else if (param_1 == 2) {
    uVar1 = 0x905;
    FUN_1000_3fa6(&stack0xfffe,0x6e,1,0x85,0,0x7000,0x89,0,0x3280,0,0,0,param_2);
    *(undefined2 *)0x2074 = 0xea7e;
    *(undefined1 *)0x799f = 5;
  }
  else if (param_1 == 3) {
    uVar1 = 0x903;
    FUN_1000_3fa6(&stack0xfffe,0x6e,9,0x85,0,0x3000,0x89,0,0x3280,0,0,0,param_2);
    *(undefined2 *)0x2074 = 0xea88;
    *(undefined1 *)0x799f = 6;
  }
  else if (param_1 == 4) {
    uVar1 = 0x3a0a;
    FUN_1000_0495(0x1aba,unaff_DS,0x1ab6,unaff_DS,0);
    FUN_1000_3fa6(&stack0xfffe,0x7e,0xffdd,0x84,0,0x7000,0x88,0,0x3400,0x87,0,0x3400,param_2);
    FUN_1000_3fa6(&stack0xfffe,0x7e,0xffdd,0x84,0,0x7000,0x87,0,0x3400,0,0,0,param_2 + 1);
    FUN_1000_3fa6(&stack0xfffe,0x7e,0xffdd,0x84,0,0x7000,0x89,0,0x700,0x88,0,0x3400,
                  param_2 + *(int *)0xc204);
    FUN_1000_3fa6(&stack0xfffe,0x7e,0xffdd,0x84,0,0x7000,0x89,0,0x3400,0x89,0,0xc00,
                  param_2 + *(int *)0xc204 + 1);
    *(undefined1 *)0x799f = 7;
    *(undefined2 *)0x2074 = 0xeace;
  }
  FUN_1000_165a(uVar1);
  return;
}


