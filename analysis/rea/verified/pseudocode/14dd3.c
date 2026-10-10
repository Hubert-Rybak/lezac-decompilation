/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x14dd3 FUN_1000_4dd3
 * ev_ccb0df3ac46f9ccfa8dde76c242a2edea6768534521cdab9888c65aee8bc30ca
 */

void __cdecl16near FUN_1000_4dd3(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  uint uVar6;
  int *piVar7;
  undefined2 uVar8;
  undefined2 unaff_DS;
  
  *(undefined2 *)0x2074 = 10000;
  *(undefined2 *)0x2072 = 0;
  *(undefined1 *)0x661e = 0;
  uVar8 = (undefined2)((ulong)*(undefined4 *)0x6612 >> 0x10);
  iVar2 = *(int *)0x2090;
  *(int *)0x2092 = iVar2;
  iVar2 = iVar2 << 1;
  *(int *)0x2090 = iVar2;
  piVar5 = (int *)(*(int *)0x2068 << 1);
  iVar4 = *(int *)0x205c;
  do {
    iVar3 = *(int *)0x205a;
    piVar7 = piVar5;
    do {
      if (*piVar7 == *(int *)0x205e) {
        piVar1 = (int *)(iVar2 + (int)piVar7);
        uVar6 = (uint)piVar7 >> 1;
        piVar7 = (int *)(uVar6 << 1);
        iVar2 = *(int *)0x2090;
        uVar8 = *(undefined2 *)0x206e;
        if ((*(char *)(*(int *)0x2092 + uVar6) != '\0') && (*piVar1 != *(int *)0x205e)) {
          *(undefined1 *)0x661e = 1;
          return;
        }
      }
      piVar7 = piVar7 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    piVar5 = (int *)((int)piVar5 + *(int *)0x2066);
    iVar4 = iVar4 + -1;
    if (iVar4 == 0) {
      return;
    }
  } while( true );
}


