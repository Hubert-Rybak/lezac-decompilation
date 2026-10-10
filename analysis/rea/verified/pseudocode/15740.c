/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x15740 FUN_1000_5740
 * ev_db27dc5b6bf1f4c523bbc561db4268a25a39386c3de4f0456e4ca44d894ba8a1
 */

void FUN_1000_5740(uint param_1)

{
  undefined2 uVar1;
  char *pcVar2;
  uint uVar3;
  uint *puVar4;
  undefined2 unaff_DS;
  uint *local_e;
  uint local_8;
  int local_6;
  char *local_4;
  
  FUN_1920_04df();
  uVar1 = *(undefined2 *)0x2074;
  *(undefined2 *)0x2074 = 0x27;
  *(undefined1 *)0x799f = 6;
  FUN_1000_165a();
  *(undefined2 *)0x2074 = uVar1;
  local_8 = 0;
  if ((*(char *)0x79a7 != '\0') && (*(byte *)0x79a7 != 0)) {
    local_4 = (char *)0x1;
    while( true ) {
      if (*(uint *)((int)local_4 * 0xe + 0x77c4) == (param_1 & 0x7fff)) {
        local_8 = (uint)local_4;
      }
      if (local_4 == (char *)(uint)*(byte *)0x79a7) break;
      local_4 = (char *)((int)local_4 + 1);
    }
  }
  if (local_8 != 0) {
    puVar4 = (uint *)(local_8 * 0xe + 0x77c0);
    _local_e = (uint *)CONCAT22(unaff_DS,puVar4);
    pcVar2 = (char *)*(int *)0x78ba;
    if (pcVar2 != (char *)0x0) {
      local_4 = (char *)0x1;
      while( true ) {
        uVar3 = *(uint *)((int)local_4 << 1) & 0x7fff;
        if ((*_local_e <= uVar3) && (uVar3 <= *(uint *)(local_8 * 0xe + 0x77c2))) {
          local_6 = 1;
          while( true ) {
            if ((*(char *)((int)puVar4 + local_6 + 5) != '\0') &&
               (*local_4 == *(char *)((int)puVar4 + local_6 + 5))) {
              *local_4 = *(char *)((int)puVar4 + local_6 + 9);
            }
            if (local_6 == 4) break;
            local_6 = local_6 + 1;
          }
        }
        if (local_4 == pcVar2) break;
        local_4 = local_4 + 1;
      }
    }
  }
  return;
}


