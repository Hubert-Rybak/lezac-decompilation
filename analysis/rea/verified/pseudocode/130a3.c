/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x130a3 FUN_1000_30a3
 * ev_4d72561f83a22766d85f68a81fb63c1c1dccaa129aa348351ebc003ffdff8666
 */

void FUN_1000_30a3(byte param_1)

{
  char *pcVar1;
  int iVar2;
  int iVar3;
  char *pcVar4;
  undefined2 unaff_DS;
  undefined4 uVar5;
  
  uVar5 = 0x100030ae;
  FUN_1920_04df();
  *(undefined2 *)0x2074 = 0x56;
  *(undefined1 *)0x799f = 5;
  FUN_1000_165a(uVar5);
  pcVar4 = (char *)0x0;
  iVar2 = *(int *)0x78ba;
  iVar3 = 0;
  do {
    pcVar1 = pcVar4;
    pcVar4 = pcVar4 + 1;
    if (*pcVar1 == *(char *)0x79b4) {
      iVar3 = iVar3 + 1;
    }
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  *(int *)0x2074 = iVar3;
  if ((uint)(*(int *)0x2088 + *(int *)0x2074) < *(uint *)0x2086) {
    *(undefined1 *)0x79ca = 0;
  }
  else {
    *(undefined1 *)0x79ca = 1;
  }
  iVar2 = (uint)param_1 * 0x26 + 0x1b62;
  FUN_1000_06ab(1,3,*(undefined1 *)0x6d,*(undefined1 *)0x6c,(uint)param_1 * 0x26 + 0x1b78,unaff_DS);
  *(undefined1 *)(iVar2 + 0x15) = 2;
  *(undefined2 *)(iVar2 + 0x10) = 0x3c;
  *(undefined1 *)(iVar2 + 0x24) = 100;
  return;
}


