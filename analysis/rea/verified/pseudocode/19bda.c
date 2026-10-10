/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19bda FUN_1920_09da
 * ev_ee670a41f50f45594f0e0bd953a69d45ee221b2254658db2deec3a9a222382cd
 */

void __stdcall16far FUN_1920_09da(byte *param_1,byte *param_2)

{
  byte bVar1;
  uint uVar2;
  byte *pbVar3;
  byte *pbVar4;
  
  pbVar4 = (byte *)param_2;
  pbVar3 = (byte *)param_1;
  bVar1 = *param_1;
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


