/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x108a5 FUN_1000_08a5
 * ev_df82dcbae1d32b18e1b9269269c18878d90f5f5872192d4fd1d18e11fff636d2
 */

void FUN_1000_08a5(int param_1,int param_2,undefined4 param_3)

{
  char *pcVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  int *local_192;
  byte local_18b;
  byte local_18a;
  byte local_189;
  int local_188;
  int local_186;
  uint local_184;
  undefined1 local_182 [128];
  undefined1 local_102 [252];
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x1000;
  uStack_6 = 0x8b0;
  FUN_1920_04df();
  FUN_1920_09f4(0xff,local_102,unaff_SS,(int)param_3,(int)((ulong)param_3 >> 0x10));
  FUN_1920_15ca(local_102,unaff_SS,local_182,unaff_SS);
  FUN_1920_15f8(1,local_182,unaff_SS);
  FUN_1920_16e3(0,0,1,&local_189,unaff_SS,local_182,unaff_SS);
  FUN_1920_16e3(0,0,(uint)local_189 * 0x26,(*(byte *)0x208d + 1) * 0x26 + 0x1bae,unaff_DS,local_182,
                unaff_SS);
  local_18a = *(byte *)0xc496;
  uVar3 = (uint)local_189;
  if (uVar3 != 0) {
    local_184 = 1;
    while( true ) {
      FUN_1920_16e3(0,0,1,&local_18b,unaff_SS,local_182,unaff_SS);
      FUN_18ac_0517(1,1,local_18b);
      if (local_184 == uVar3) break;
      local_184 = local_184 + 1;
    }
  }
  uVar3 = (uint)local_189;
  if (uVar3 != 0) {
    local_184 = 1;
    while( true ) {
      FUN_1920_16e3(0,0,2,&local_186,unaff_SS,local_182,unaff_SS);
      FUN_1920_16e3(0,0,2,&local_188,unaff_SS,local_182,unaff_SS);
      iVar4 = (local_18a + local_184 + -1) * 8;
      _local_192 = (int *)CONCAT22(unaff_DS,(int *)(iVar4 + -0x3de2));
      *_local_192 = param_2 + local_186;
      *(int *)(iVar4 + -0x3de0) = param_1 + local_188;
      if (local_184 == uVar3) break;
      local_184 = local_184 + 1;
    }
  }
  FUN_1920_16e3(0,0,1,&local_18b,unaff_SS,local_182,unaff_SS);
  *(int *)((*(byte *)0x208d + 1) * 0x26 + 0x1bc0) = *(byte *)0x208d + 1;
  if (1 < local_189) {
    local_184 = 2;
    while( true ) {
      iVar4 = (*(byte *)0x208d + local_184) * 0x26;
      if ((((*(uint *)(iVar4 + 0x1bbc) & 0xff) != 0) &&
          (*(int *)(iVar4 + 0x1bbc) = *(int *)(iVar4 + 0x1bbc) + (uint)*(byte *)0x79f9,
          *(uint *)(iVar4 + 0x1bbc) >> 8 != 0)) &&
         (*(int *)(iVar4 + 0x1bbc) = *(int *)(iVar4 + 0x1bbc) + (uint)*(byte *)0x79f9 * 0x100,
         (*(uint *)(iVar4 + 0x1bbe) & 0xff) != 0)) {
        *(int *)(iVar4 + 0x1bbe) = *(int *)(iVar4 + 0x1bbe) + (uint)*(byte *)0x79f9;
      }
      *(char *)(iVar4 + 0x1bd3) = *(char *)0x208d + '\x01';
      if (local_184 == local_189) break;
      local_184 = local_184 + 1;
    }
  }
  FUN_1920_16e3(0,0,(uint)local_18b << 4,(*(byte *)0x79f9 + 1) * 0x10 + 0x79ea,unaff_DS,local_182,
                unaff_SS);
  iVar4 = FUN_1920_04a2();
  if (iVar4 != 0) {
    FUN_1000_00a3(1);
  }
  bVar2 = *(byte *)0x79f9;
  local_184 = *(byte *)0x79f9 + 1;
  if (local_184 <= (uint)bVar2 + (uint)local_18b) {
    while( true ) {
      _local_192 = (int *)CONCAT22(unaff_DS,(char *)(local_184 * 0x10 + 0x79ea));
      *(byte *)_local_192 = *(char *)_local_192 + local_18a;
      pcVar1 = (char *)(local_184 * 0x10 + 0x79eb);
      *pcVar1 = *pcVar1 + local_18a;
      if (local_184 == (uint)bVar2 + (uint)local_18b) break;
      local_184 = local_184 + 1;
    }
  }
  *(char *)0x79f9 = *(char *)0x79f9 + local_18b;
  uVar3 = (uint)local_189;
  if (uVar3 != 0) {
    local_184 = 1;
    while( true ) {
      iVar4 = (*(byte *)0x208d + local_184) * 0x26;
      if (1 < (int)local_184) {
        *(char *)(iVar4 + 0x1bb0) = *(char *)(iVar4 + 0x1bb0) + local_18a;
      }
      *(char *)(iVar4 + 0x1baf) = *(char *)(iVar4 + 0x1baf) + local_18a;
      if (*(char *)(iVar4 + 0x1bb1) != '\0') {
        FUN_1000_06ab(1,*(undefined1 *)(iVar4 + 0x1bc8),
                      *(undefined1 *)((uint)*(byte *)(iVar4 + 0x1bb1) * 2 + 0x59),
                      *(undefined1 *)((uint)*(byte *)(iVar4 + 0x1bb1) * 2 + 0x58),iVar4 + 0x1bc4,
                      unaff_DS);
      }
      if (local_184 == uVar3) break;
      local_184 = local_184 + 1;
    }
  }
  *(char *)0x208d = *(char *)0x208d + local_189;
  return;
}


