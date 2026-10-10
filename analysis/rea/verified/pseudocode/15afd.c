/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x15afd FUN_1000_5afd
 * ev_18c258dbc18e00405c199f68a3b224bb38661072362a74de7144ec7df5849022
 */

void FUN_1000_5afd(void)

{
  undefined2 uVar1;
  char *pcVar2;
  uint uVar3;
  char cVar4;
  uint uVar5;
  uint *puVar6;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  *(undefined1 *)0x79a3 = 0;
  *(undefined2 *)0x2072 = 0;
  *(undefined1 *)0x661e = 0;
  pcVar2 = (char *)*(undefined2 *)0xc1e8;
  cVar4 = *pcVar2;
  if (('f' < cVar4) && (cVar4 < 's')) {
    *(char *)0x79a3 = cVar4;
    uVar5 = (uint)(byte)((cVar4 + -0x67) * '\x02');
    *(undefined2 *)0x2072 = *(undefined2 *)(uVar5 + 0x1a);
    *(int *)0x2074 = *(int *)0x2074 + *(int *)(uVar5 + 2);
    uVar1 = *(undefined2 *)0x206e;
    uVar5 = *(uint *)((int)pcVar2 * 2);
    puVar6 = (uint *)((int)pcVar2 * 2 - *(int *)0x2066);
    uVar3 = *puVar6;
    if ((uVar3 != 0) && (uVar3 < 0x8000)) {
      *(undefined1 *)0x661e = 1;
    }
    if (uVar5 < 0x8000) {
      cVar4 = '\0';
      *(undefined2 *)((int)puVar6 + *(int *)0x2066) = 0;
    }
    else {
      cVar4 = -1;
    }
    *pcVar2 = cVar4;
  }
  return;
}


