/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x129db FUN_1000_29db
 * ev_3e9cb6b0fd6d7aa8db8271589cbf00436cfe3e3115e5fa1245e85ada9ab6cb68
 */

void FUN_1000_29db(int param_1)

{
  byte bVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 local_4;
  
  FUN_1920_04df();
  bVar1 = *(byte *)(param_1 + 0x79e9);
  iVar2 = *(int *)(param_1 * 2 + 0x79f2);
  *(int *)0x2074 = *(int *)0xc208 + 0x1cc0;
  *(int *)0x2072 = iVar2 + -0x1000;
  local_4 = 1;
  while( true ) {
    if ((int)(uint)bVar1 < local_4) {
      *(undefined2 *)0x2074 = *(undefined2 *)0xc208;
    }
    FUN_1000_1156();
    *(int *)0x2072 = *(int *)0x2072 + 9;
    if (local_4 == 3) break;
    local_4 = local_4 + 1;
  }
  return;
}


