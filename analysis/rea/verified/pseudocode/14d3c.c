/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x14d3c FUN_1000_4d3c
 * ev_39e1fed34bb53b0c3f6ca45d126556d163b2d62fc0ee62042560b9a266fe6faf
 */

uint __cdecl16near FUN_1000_4d3c(void)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint *puVar6;
  uint uVar7;
  uint *puVar8;
  undefined2 uVar9;
  undefined2 unaff_DS;
  
  *(undefined2 *)0x2074 = 10000;
  *(undefined2 *)0x2072 = 0;
  *(undefined1 *)0x661e = 0;
  uVar9 = (undefined2)((ulong)*(undefined4 *)0x6612 >> 0x10);
  iVar2 = *(int *)0x2090;
  *(int *)0x2092 = iVar2;
  iVar2 = iVar2 << 1;
  *(int *)0x2090 = iVar2;
  puVar6 = (uint *)(*(int *)0x2068 << 1);
  iVar5 = *(int *)0x205c;
  do {
    iVar4 = *(int *)0x205a;
    puVar8 = puVar6;
    do {
      uVar3 = *puVar8;
      if (uVar3 == *(uint *)0x205e) {
        piVar1 = (int *)(iVar2 + (int)puVar8);
        uVar7 = (uint)puVar8 >> 1;
        uVar3 = CONCAT11((char)(uVar3 >> 8),*(char *)(*(int *)0x2092 + uVar7));
        puVar8 = (uint *)(uVar7 << 1);
        iVar2 = *(int *)0x2090;
        uVar9 = *(undefined2 *)0x206e;
        if ((*(char *)(*(int *)0x2092 + uVar7) != '\0') && (*piVar1 != *(int *)0x205e)) {
          *(undefined1 *)0x661e = 1;
          uVar3 = (uVar7 + *(int *)0x2092) / *(uint *)0xc204;
          uVar7 = (uVar7 + *(int *)0x2092) % *(uint *)0xc204;
          if ((int)uVar7 < *(int *)0x2074) {
            *(uint *)0x2074 = uVar7;
          }
          if (*(int *)0x2072 < (int)uVar7) {
            *(uint *)0x2072 = uVar7;
          }
        }
      }
      puVar8 = puVar8 + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    puVar6 = (uint *)((int)puVar6 + *(int *)0x2066);
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  return uVar3;
}


