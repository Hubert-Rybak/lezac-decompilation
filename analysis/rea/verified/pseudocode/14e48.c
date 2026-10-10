/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x14e48 FUN_1000_4e48
 * ev_31147bbfbbfc193ee64c7b4e7fd9a56859fe6089de31ad1f9a5e6cb650675f01
 */

int __cdecl16near FUN_1000_4e48(void)

{
  int *piVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int *piVar10;
  int *piVar11;
  undefined2 uVar12;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x661e = 0;
  uVar12 = (undefined2)((ulong)*(undefined4 *)0x6612 >> 0x10);
  iVar5 = *(int *)0x2090;
  *(int *)0x2092 = iVar5;
  iVar5 = iVar5 << 1;
  *(int *)0x2090 = iVar5;
  piVar10 = (int *)(*(int *)0x2068 << 1);
  iVar8 = *(int *)0x205c;
  do {
    iVar7 = *(int *)0x205a;
    piVar11 = piVar10;
    do {
      iVar6 = *piVar11;
      if (iVar6 == *(int *)0x205e) {
        iVar3 = *(int *)(iVar5 + (int)piVar11);
        cVar2 = *(char *)(*(int *)0x2092 + ((uint)piVar11 >> 1));
        iVar6 = CONCAT11((char)((uint)iVar6 >> 8),cVar2);
        piVar11 = (int *)(((uint)piVar11 >> 1) * 2);
        iVar5 = *(int *)0x2090;
        uVar12 = *(undefined2 *)0x206e;
        if (((cVar2 != '\0') && (iVar3 != *(int *)0x205e)) &&
           (*(undefined1 *)0x661e = 1, iVar3 != 0)) {
          iVar6 = (int)piVar11 + iVar5;
          iVar9 = *(int *)0x2078 * 2;
          iVar4 = iVar9;
          do {
            if (iVar4 == 0) {
              *(int *)(iVar9 + 0x655e) = iVar3;
              *(int *)(iVar9 + 0x659a) = iVar6;
              *(int *)0x2078 = *(int *)0x2078 + 1;
              break;
            }
            piVar1 = (int *)(iVar4 + 0x655c);
            iVar4 = iVar4 + -2;
          } while (*piVar1 != iVar3);
        }
      }
      piVar11 = piVar11 + 1;
      iVar7 = iVar7 + -1;
    } while (iVar7 != 0);
    piVar10 = (int *)((int)piVar10 + *(int *)0x2066);
    iVar8 = iVar8 + -1;
    if (iVar8 == 0) {
      return iVar6;
    }
  } while( true );
}


