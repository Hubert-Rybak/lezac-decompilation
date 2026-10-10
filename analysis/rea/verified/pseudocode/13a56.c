/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x13a56 FUN_1000_3a56
 * ev_96c557ea89afa7534319f189ae973bccaed31d3f4dd8ecf4a120f7980cfa640b
 */

void __cdecl16near FUN_1000_3a56(void)

{
  ulong uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  bool bVar3;
  
  uVar1 = (ulong)(*(int *)0x2076 - 1) * 0xb;
  uVar2 = (uint)uVar1;
  do {
    if (*(int *)(uVar2 + 0x209e) == *(int *)0x2072) {
      *(int *)0x2074 = (int)((uVar1 & 0xffff0000 | (ulong)uVar2) / 0xb) + 1;
      return;
    }
    bVar3 = 10 < uVar2;
    uVar2 = uVar2 - 0xb;
  } while (bVar3);
  return;
}


