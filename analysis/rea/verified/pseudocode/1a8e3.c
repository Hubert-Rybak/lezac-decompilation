/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a8e3 FUN_1920_16e3
 * ev_0a60ca64187a036a91cf5fc0071d0a28d8b4a2d782a76debae82860a6aa924f5
 */

void __stdcall16far
FUN_1920_16e3(uint *param_1,uint param_2,undefined2 param_3,undefined2 param_4,undefined4 param_5)

{
  code *pcVar1;
  uint uVar2;
  undefined2 uVar3;
  undefined2 unaff_BP;
  undefined2 uVar4;
  int iVar5;
  undefined2 unaff_DS;
  bool bVar6;
  undefined1 in_ZF;
  undefined2 in_stack_00000000;
  
  uVar3 = 100;
  uVar4 = (undefined2)((ulong)param_5 >> 0x10);
  FUN_1920_169e();
  iVar5 = (int)((ulong)param_1 >> 0x10);
  if ((bool)in_ZF) {
    uVar2 = 0;
    if (param_2 != 0) {
      bVar6 = (int)((ulong)param_2 * (ulong)*(uint *)((int)param_5 + 4) >> 0x10) != 0;
      pcVar1 = (code *)swi(0x21);
      uVar2 = (*pcVar1)();
      if (bVar6) {
        *(uint *)0x1afc = uVar2;
        goto LAB_1920_1739;
      }
      uVar2 = uVar2 / *(uint *)((int)param_5 + 4);
      uVar3 = unaff_BP;
      unaff_DS = in_stack_00000000;
    }
    if (iVar5 == 0 && (uint *)param_1 == (uint *)0x0) {
      if (uVar2 != param_2) {
        *(undefined2 *)0x1afc = uVar3;
      }
    }
    else {
      *param_1 = uVar2;
    }
  }
  else {
LAB_1920_1739:
    if (iVar5 != 0 || (uint *)param_1 != (uint *)0x0) {
      *param_1 = 0;
    }
  }
  return;
}


