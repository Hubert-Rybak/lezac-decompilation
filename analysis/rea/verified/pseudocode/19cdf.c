/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19cdf FUN_1920_0adf
 * ev_8328b445cf3f37cd7695d02620cfce920def7dbba1e61ad7ba7f1eca2110335f
 */

void __stdcall16far FUN_1920_0adf(byte *param_1,byte *param_2)

{
  byte bVar1;
  uint uVar2;
  byte *pbVar3;
  byte *pbVar4;
  
  pbVar3 = (byte *)param_2;
  pbVar4 = (byte *)param_1;
  bVar1 = *param_2;
  if (*param_1 < *param_2) {
    bVar1 = *param_1;
  }
  if (bVar1 != 0) {
    uVar2 = (uint)bVar1;
    do {
      pbVar4 = pbVar4 + 1;
      pbVar3 = pbVar3 + 1;
      if (uVar2 == 0) {
        return;
      }
      uVar2 = uVar2 - 1;
    } while (*pbVar3 == *pbVar4);
  }
  return;
}


