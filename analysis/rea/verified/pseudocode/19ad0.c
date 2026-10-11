/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19ad0 FUN_1920_08d0
 * ev_3b87443e4a278a455ab29782b6cd673dec489f2446461b82fe38e723734ad4d7
 */

void __stdcall16far FUN_1920_08d0(int param_1,byte *param_2,undefined4 param_3)

{
  int iVar1;
  uint uVar2;
  undefined2 in_BX;
  undefined1 in_ZF;
  
  FUN_1920_07b8();
  if ((bool)in_ZF) {
    uVar2 = (uint)*param_2;
    iVar1 = param_1 - uVar2;
    if (iVar1 != 0 && (int)uVar2 <= param_1) {
      do {
        FUN_1920_07dc();
        iVar1 = iVar1 + -1;
      } while (iVar1 != 0);
      uVar2 = (uint)*param_2;
    }
    for (; uVar2 != 0; uVar2 = uVar2 - 1) {
      FUN_1920_07dc();
    }
  }
  *(undefined2 *)((int)param_3 + 8) = in_BX;
  return;
}


