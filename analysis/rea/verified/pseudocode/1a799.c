/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a799 FUN_1920_1599
 * ev_2cb8fc11d3bba19e168501b772f1d011e32adb37306e59ecb5e3253e3f7f0aba
 */

undefined2 __stdcall16far FUN_1920_1599(int *param_1,byte *param_2)

{
  undefined2 uVar1;
  uint uVar2;
  int iVar3;
  byte *pbVar4;
  bool bVar5;
  
  bVar5 = false;
  uVar2 = (uint)*param_2;
  pbVar4 = (byte *)param_2;
  while ((pbVar4 = pbVar4 + 1, uVar2 != 0 && (bVar5 = *pbVar4 < 0x20, *pbVar4 == 0x20))) {
    uVar2 = uVar2 - 1;
  }
  uVar1 = FUN_1920_14b6();
  if ((bVar5) || (iVar3 = 0, uVar2 != 0)) {
    iVar3 = (int)pbVar4 - (int)(byte *)param_2;
    uVar1 = 0;
  }
  *param_1 = iVar3;
  return uVar1;
}


