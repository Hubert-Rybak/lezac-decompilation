/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a7ca FUN_1920_15ca
 * ev_27d5675da0c6316e3aab6d85198a670c9e14a79df1263a5431d4dbf818bbd249
 */

void __stdcall16far FUN_1920_15ca(byte *param_1,undefined2 *param_2)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  byte *pbVar5;
  byte *pbVar6;
  undefined2 uVar7;
  
  uVar7 = (undefined2)((ulong)param_2 >> 0x10);
  pbVar5 = (byte *)param_1;
  pbVar6 = (byte *)((undefined2 *)param_2 + 1);
  *param_2 = 0;
  pbVar6[0] = 0xb0;
  pbVar6[1] = 0xd7;
  for (iVar3 = 0x16; pbVar6 = pbVar6 + 2, iVar3 != 0; iVar3 = iVar3 + -1) {
    pbVar6[0] = 0;
    pbVar6[1] = 0;
  }
  bVar2 = *param_1;
  if (0x4f < bVar2) {
    bVar2 = 0x4f;
  }
  for (uVar4 = (uint)bVar2; pbVar5 = pbVar5 + 1, uVar4 != 0; uVar4 = uVar4 - 1) {
    pbVar1 = pbVar6;
    pbVar6 = pbVar6 + 1;
    *pbVar1 = *pbVar5;
  }
  *pbVar6 = 0;
  return;
}


