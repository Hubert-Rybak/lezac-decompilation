/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x14fda FUN_1000_4fda
 * ev_7da5831fb5d59721ee53fe17dbcd0a5022cb697df6dac390301a461eecae727e
 */

void __cdecl16near FUN_1000_4fda(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  char *pcVar5;
  int *piVar6;
  int *piVar7;
  undefined2 uVar8;
  undefined2 unaff_DS;
  undefined2 uVar9;
  
  FUN_1920_04df();
  uVar8 = (undefined2)((ulong)*(undefined4 *)0x6612 >> 0x10);
  piVar7 = (int *)*(undefined2 *)0x2068;
  iVar4 = *(int *)0x205c;
  do {
    iVar3 = *(int *)0x205a;
    piVar6 = piVar7;
    do {
      if (*piVar6 == *(int *)0x205e) {
        *piVar6 = *(int *)0x78c4;
        *(int *)0x78c8 = *(int *)0x78c8 + 1;
        pcVar5 = (char *)((uint)piVar6 >> 1);
        *pcVar5 = ((byte)*(undefined2 *)0x78c2 & 2) + 0x47;
        *(undefined2 *)0x2074 = pcVar5;
        piVar6 = (int *)((int)pcVar5 << 1);
        *(int *)0x78c4 = *(int *)0x78c4 + 1;
        uVar9 = *(undefined2 *)0x2074;
        iVar1 = FUN_1920_13a8(0x14);
        iVar1 = *(char *)0x78d2 + iVar1 + -10;
        iVar2 = FUN_1920_13a8(0x28);
        FUN_1000_370e(0x79a3,unaff_DS,*(char *)0x78d4 - iVar2,iVar1,uVar9);
      }
      piVar6 = piVar6 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    piVar7 = (int *)((int)piVar7 + *(int *)0x2066);
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return;
}


