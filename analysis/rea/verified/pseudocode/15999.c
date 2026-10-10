/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x15999 FUN_1000_5999
 * ev_92159ec5ea861395e68b32ef5c5b1a26a32b0fc1b7e2cedd0bb84cd0e98a2a7d
 */

void FUN_1000_5999(int param_1,uint param_2)

{
  int iVar1;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  uint *local_8;
  int local_4;
  
  FUN_1920_04df();
  local_4 = 1;
  *(undefined1 *)0x1b7c = 0;
  *(undefined1 *)0x1b81 = 0;
  while( true ) {
    if ((int)(uint)*(byte *)0x79a5 < local_4) {
      return;
    }
    iVar1 = local_4 * 7;
    _local_8 = (uint *)CONCAT22(unaff_DS,(uint *)(iVar1 + 0x7717));
    if (*_local_8 == (param_2 & 0x7fff)) break;
    local_4 = local_4 + 1;
  }
  *(undefined2 *)(param_1 + -0x2c) = *(undefined2 *)(iVar1 + 0x7719);
  *(undefined2 *)(param_1 + -0x2e) = *(undefined2 *)(iVar1 + 0x771b);
  *(undefined1 *)(param_1 + -0x12) = *(undefined1 *)0x6c;
  *(undefined2 *)0x2074 = 0x1a;
  *(undefined1 *)0x799f = 4;
  FUN_1000_165a();
  FUN_1000_2f9f(5,8,0xb,*(undefined1 *)(param_1 + -0x12),0,0,*(undefined2 *)(param_1 + -0x2e),
                *(undefined2 *)(param_1 + -0x2c));
  if (*(int *)0x2072 != 1) {
    return;
  }
  FUN_1000_06ab(1,2,*(undefined1 *)0x6d,*(undefined1 *)(param_1 + -0x12),
                (uint)*(byte *)0x208d * 0x26 + 0x1bc4,unaff_DS);
  return;
}


