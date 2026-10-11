/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a879 FUN_1920_1679
 * ev_f5516ff9681217e4957a6fc83aa4798c443d9bcb87cf9555187f72efb10679ee
 */

void __stdcall16far FUN_1920_1679(uint *param_1)

{
  code *pcVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  bool bVar3;
  undefined1 in_ZF;
  
  FUN_1920_169e();
  if ((bool)in_ZF) {
    bVar3 = *param_1 < 4;
    if (4 < *param_1) {
      pcVar1 = (code *)swi(0x21);
      uVar2 = (*pcVar1)();
      if (bVar3) {
        *(undefined2 *)0x1afc = uVar2;
      }
    }
    ((uint *)param_1)[1] = 0xd7b0;
  }
  return;
}


