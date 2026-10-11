/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x156b6 FUN_1000_56b6
 * ev_f4eb515b845ec3b0a95cd30bef945a7c77ff723d24b8db7366b0f5f8b8536a01
 */

void __cdecl16near FUN_1000_56b6(void)

{
  undefined2 uVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  byte *pbVar5;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x661e = 0;
  iVar4 = 4;
  iVar3 = *(int *)0x2072;
  *(undefined2 *)0x2072 = 0;
  uVar1 = *(undefined2 *)0xc1fe;
  pbVar5 = (byte *)*(undefined2 *)0x2074;
  do {
    bVar2 = *pbVar5;
    if (bVar2 == 0x75) {
      *(undefined2 *)0x2072 = pbVar5;
      *(char *)0x661e = *(char *)0x661e + -1;
      *(char *)0x661e = *(char *)0x661e + -1;
    }
    if (((iVar3 < iVar4) && (bVar2 != 0)) && (bVar2 < 0x4d)) {
      *(char *)0x661e = *(char *)0x661e - *(char *)0x208c;
    }
    if (iVar4 == 4) {
      pbVar5 = pbVar5 + 1;
    }
    else if (iVar4 == 3) {
      pbVar5 = pbVar5 + *(int *)0xc204;
    }
    else if (iVar4 == 2) {
      pbVar5 = pbVar5 + -1;
    }
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return;
}


