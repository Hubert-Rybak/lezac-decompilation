/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x10709 FUN_1000_0709
 * ev_b7f5a489150c06512b2ec4e97f2ad84986e4de6900cad333d3cf7a8831d3191b
 */

void FUN_1000_0709(undefined4 param_1)

{
  code *pcVar1;
  int iVar2;
  undefined2 unaff_SS;
  undefined1 local_484 [770];
  undefined1 local_182 [128];
  undefined1 local_102 [252];
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x1000;
  uStack_6 = 0x714;
  FUN_1920_04df();
  FUN_1920_09f4(0xff,local_102,unaff_SS,(int)param_1,(int)((ulong)param_1 >> 0x10));
  FUN_1920_15ca(local_102,unaff_SS,local_182,unaff_SS);
  FUN_1920_15f8(1,local_182,unaff_SS);
  FUN_1920_16e3(0,0,0x300,local_484,unaff_SS,local_182,unaff_SS);
  FUN_1920_1679(local_182,unaff_SS);
  iVar2 = FUN_1920_04a2();
  if (iVar2 != 0) {
    FUN_1000_00a3(1);
  }
  pcVar1 = (code *)swi(0x10);
  (*pcVar1)();
  return;
}


