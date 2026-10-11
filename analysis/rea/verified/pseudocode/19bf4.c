/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19bf4 FUN_1920_09f4
 * ev_dce23d3f08f8fd2aaca94e70d82afa6e7c3e3858e8a4f28e02e5b4d9faf7e914
 */

void __stdcall16far FUN_1920_09f4(byte param_1,byte *param_2,byte *param_3)

{
  byte bVar1;
  uint uVar2;
  byte *pbVar3;
  byte *pbVar4;
  
  pbVar3 = (byte *)param_3;
  pbVar4 = (byte *)param_2;
  bVar1 = *param_3;
  if (param_1 < *param_3) {
    bVar1 = param_1;
  }
  *param_2 = bVar1;
  uVar2 = (uint)bVar1;
  while( true ) {
    pbVar4 = pbVar4 + 1;
    pbVar3 = pbVar3 + 1;
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    *pbVar4 = *pbVar3;
  }
  return;
}


