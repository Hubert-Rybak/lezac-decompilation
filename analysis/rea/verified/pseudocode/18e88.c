/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x18e88 FUN_18ac_03c8
 * ev_32ac88fdc06e73d9b6302c983a72fcaaf099c7837f456d181c5963c90588dbc6
 */

void __cdecl16far FUN_18ac_03c8(void)

{
  undefined2 uVar1;
  undefined2 *puVar2;
  undefined2 *puVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined2 *puVar8;
  undefined2 *puVar9;
  undefined2 unaff_DS;
  
  puVar9 = (undefined2 *)*(undefined2 *)0xc1f4;
  puVar8 = (undefined2 *)(*(int *)0xc214 + *(int *)0xc1ec * *(int *)0xc20c + *(int *)0xc20a);
  uVar7 = *(int *)0xc1ec - 8U >> 1;
  iVar4 = *(int *)0xc206;
  iVar5 = *(int *)0xc1ee + -8;
  uVar1 = *(undefined2 *)0xc212;
  uVar6 = uVar7;
  do {
    for (; uVar6 != 0; uVar6 = uVar6 - 1) {
      puVar3 = puVar9;
      puVar9 = puVar9 + 1;
      puVar2 = puVar8;
      puVar8 = puVar8 + 1;
      *puVar3 = *puVar2;
    }
    puVar9 = (undefined2 *)((int)puVar9 + iVar4 + 8);
    puVar8 = puVar8 + 4;
    iVar5 = iVar5 + -1;
    uVar6 = uVar7;
  } while (iVar5 != 0);
  return;
}


