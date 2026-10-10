/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1508b FUN_1000_508b
 * ev_8216fa231e9710c46a7c3c02f4a58bf54f51289d039c835722a5e02ffce5d9eb
 */

void FUN_1000_508b(undefined2 param_1,uint param_2)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  undefined1 *puVar5;
  undefined1 *puVar6;
  uint *puVar7;
  undefined2 uVar8;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  if (param_2 < *(uint *)0x2080) {
    puVar6 = (undefined1 *)(*(int *)0x207a + (param_2 - 1) * 0xf);
    puVar5 = puVar6 + 0xf;
    for (iVar4 = (*(int *)0x2080 - param_2) * 0xf; iVar4 != 0; iVar4 = iVar4 + -1) {
      puVar2 = puVar6;
      puVar6 = puVar6 + 1;
      puVar1 = puVar5;
      puVar5 = puVar5 + 1;
      *puVar2 = *puVar1;
    }
  }
  uVar8 = (undefined2)((ulong)*(undefined4 *)0x6612 >> 0x10);
  do {
    puVar7 = (uint *)*(undefined2 *)0x2068;
    do {
      if (*puVar7 == *(uint *)0x205e) {
        *puVar7 = *puVar7 & 0x7fff;
      }
      puVar7 = puVar7 + 1;
    } while ((int)puVar7 <= *(int *)0x206a);
    *(int *)0x2068 = *(int *)0x2068 + *(int *)0x2066;
    iVar4 = *(int *)0x206a;
    iVar3 = *(int *)0x2066;
    *(int *)0x206a = iVar4 + iVar3;
  } while (iVar4 + iVar3 <= *(int *)0x206c);
  *(int *)0x2080 = *(int *)0x2080 + -1;
  return;
}


