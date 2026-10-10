/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x15bcc FUN_1000_5bcc
 * ev_e037409860cb9bc2a63fc98a45e55ebe8f400a9bcddb513de55dc0344b63a560
 */

void FUN_1000_5bcc(int param_1)

{
  byte bVar1;
  uint uVar2;
  undefined4 uVar3;
  char cVar4;
  int iVar5;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 local_c;
  undefined2 local_4;
  
  FUN_1920_04df();
  uVar3 = *(undefined4 *)(*(int *)(param_1 + 4) + 4);
  uVar2 = *(uint *)((int)uVar3 + 0x12);
  bVar1 = *(byte *)0x208d;
  if (bVar1 != 0) {
    local_4 = 1;
    while( true ) {
      iVar5 = local_4 * 0x26;
      _local_c = (char *)CONCAT22(unaff_DS,(char *)(iVar5 + 0x1bae));
      if ((*_local_c == '\x1f') && (*(byte *)(iVar5 + 0x1bd3) == uVar2)) {
        *_local_c = '\x0e';
        cVar4 = FUN_1920_13a8(10);
        *(char *)(iVar5 + 0x1bb0) = cVar4 + '(';
        *(undefined1 *)(iVar5 + 0x1bc3) = 2;
        *(undefined1 *)(iVar5 + 0x1bc9) = 0;
      }
      if (local_4 == bVar1) break;
      local_4 = local_4 + 1;
    }
  }
  uVar3 = *(undefined4 *)(*(int *)(param_1 + 4) + 4);
  *(undefined1 *)((int)uVar3 + 0x15) = 2;
  *(undefined1 *)*(undefined4 *)(*(int *)(param_1 + 4) + 4) = 0xe;
  uVar3 = *(undefined4 *)(*(int *)(param_1 + 4) + 4);
  *(undefined1 *)((int)uVar3 + 2) = 0x3c;
  uVar3 = *(undefined4 *)(*(int *)(param_1 + 4) + 4);
  *(undefined1 *)((int)uVar3 + 0x1b) = 0;
  FUN_1000_5740(1000);
  *(undefined2 *)0x2074 = 0x3d;
  *(undefined1 *)0x799f = 0xc;
  FUN_1000_165a();
  return;
}


