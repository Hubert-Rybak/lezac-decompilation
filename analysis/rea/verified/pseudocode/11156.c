/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x11156 FUN_1000_1156
 * ev_335e872044522566faef7fbf35f8027fc52bd3e097f974e90efb3b943080fc74
 */

void __cdecl16near FUN_1000_1156(void)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  int iVar3;
  int iVar4;
  undefined2 *puVar5;
  undefined2 *puVar6;
  undefined2 unaff_DS;
  
  puVar6 = (undefined2 *)*(undefined2 *)0x2072;
  iVar3 = 8;
  puVar5 = (undefined2 *)*(undefined2 *)0x2074;
  do {
    for (iVar4 = 4; iVar4 != 0; iVar4 = iVar4 + -1) {
      puVar2 = puVar6;
      puVar6 = puVar6 + 1;
      puVar1 = puVar5;
      puVar5 = puVar5 + 1;
      *puVar2 = *puVar1;
    }
    puVar6 = puVar6 + 0x9c;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}


