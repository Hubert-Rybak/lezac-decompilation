/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x196f7 FUN_1920_04f7
 * ev_ca79b2a601907fd892ae51a1889103f939d3603881fe76a1de4eff985326ee93
 */

void __stdcall16far FUN_1920_04f7(byte *param_1,undefined2 *param_2)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  byte *pbVar5;
  undefined2 *puVar6;
  byte *pbVar7;
  undefined2 uVar8;
  
  uVar8 = (undefined2)((ulong)param_2 >> 0x10);
  puVar6 = (undefined2 *)param_2;
  pbVar5 = (byte *)param_1;
  *param_2 = 0;
  puVar6[1] = 0xd7b0;
  puVar6[2] = 0x80;
  puVar6[3] = 0;
  puVar6[4] = 0;
  puVar6[5] = 0;
  puVar6[6] = puVar6 + 0x40;
  puVar6[7] = uVar8;
  pbVar7 = (byte *)(puVar6 + 9);
  puVar6[8] = 0x60e;
  pbVar7[0] = 0x20;
  pbVar7[1] = 0x19;
  for (iVar3 = 0xe; pbVar7 = pbVar7 + 2, iVar3 != 0; iVar3 = iVar3 + -1) {
    pbVar7[0] = 0;
    pbVar7[1] = 0;
  }
  bVar2 = *param_1;
  if (0x4f < bVar2) {
    bVar2 = 0x4f;
  }
  for (uVar4 = (uint)bVar2; pbVar5 = pbVar5 + 1, uVar4 != 0; uVar4 = uVar4 - 1) {
    pbVar1 = pbVar7;
    pbVar7 = pbVar7 + 1;
    *pbVar1 = *pbVar5;
  }
  *pbVar7 = 0;
  return;
}


