/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x15cb0 FUN_1000_5cb0
 * ev_a0ed8e74c24114f36e906f8c2078ecaf924cfba91d4365ac4159d6a5096d15e7
 */

void FUN_1000_5cb0(int param_1)

{
  char *pcVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  byte bVar5;
  uint uVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  uint local_e;
  uint local_c;
  int local_8;
  
  FUN_1920_04df();
  uVar8 = (undefined2)((ulong)*(undefined4 *)(param_1 + 4) >> 0x10);
  iVar4 = (int)*(undefined4 *)(param_1 + 4);
  uVar3 = *(uint *)(iVar4 + 0xe);
  bVar5 = (byte)((uint)*(undefined2 *)(iVar4 + 0xe) >> 8);
  *(int *)(param_1 + -0x2a) =
       (*(uint *)(param_1 + -0x2c) >> 3) + (*(uint *)(param_1 + -0x2e) >> 3) * *(int *)0xc204;
  local_8 = *(int *)(param_1 + -0x2a) - *(int *)0xc204;
  local_c = 1;
  *(undefined1 *)(param_1 + -0x22) = 0;
  for (; local_c <= (uVar3 & 0xff); local_c = local_c + 1) {
    bVar2 = *(byte *)((int)*(undefined4 *)0xc1e0 + local_8);
    if ((bVar2 != 0) && (bVar2 < 0x4d)) {
      *(undefined1 *)(param_1 + -0x22) = 1;
      break;
    }
    local_8 = local_8 + 1;
  }
  local_8 = (uint)bVar5 * *(int *)0xc204 + *(int *)(param_1 + -0x2a);
  local_c = 1;
  *(undefined1 *)(param_1 + -0x21) = 0;
  for (; local_c <= (uVar3 & 0xff); local_c = local_c + 1) {
    bVar2 = *(byte *)((int)*(undefined4 *)0xc1e0 + local_8);
    if ((bVar2 != 0) && (bVar2 < 0x53)) {
      *(undefined1 *)(param_1 + -0x21) = 1;
      break;
    }
    local_8 = local_8 + 1;
  }
  local_8 = *(int *)(param_1 + -0x2a) + -1;
  local_c = 1;
  *(undefined1 *)(param_1 + -0x23) = 0;
  for (; local_c <= bVar5; local_c = local_c + 1) {
    bVar2 = *(byte *)((int)*(undefined4 *)0xc1e0 + local_8);
    if ((bVar2 != 0) && (bVar2 < 0x4d)) {
      *(undefined1 *)(param_1 + -0x23) = 1;
      break;
    }
    local_8 = local_8 + *(int *)0xc204;
  }
  local_8 = (uVar3 & 0xff) + *(int *)(param_1 + -0x2a);
  local_c = 1;
  *(undefined1 *)(param_1 + -0x24) = 0;
  do {
    if (bVar5 < local_c) {
LAB_1000_5e59:
      if (*(uint *)0x78c2 % 0x1d == 0) {
        uVar6 = FUN_1920_13a8(100);
        if ((0x46 < uVar6) && ((*(uint *)0x78c2 & 1) == 0)) {
          *(undefined2 *)0x2074 = 0x69;
          *(undefined1 *)0x799f = 4;
          FUN_1000_165a();
        }
        if (*(int *)(param_1 + -4) < 1) {
          iVar7 = FUN_1920_13a8(800);
          *(int *)(param_1 + -0xc) = -0x96 - iVar7;
        }
        else {
          iVar7 = FUN_1920_13a8(800);
          *(int *)(param_1 + -0xc) = iVar7 + 0x96;
        }
        if (*(char *)(param_1 + -0x21) != '\0') {
          iVar7 = FUN_1920_13a8(0x5dc);
          *(int *)(param_1 + -0xe) = -300 - iVar7;
        }
      }
      *(undefined1 *)0x661e = 0;
      local_8 = *(int *)(param_1 + -0x2a);
      for (local_c = 1; iVar7 = local_8, local_c <= bVar5; local_c = local_c + 1) {
        for (local_e = 1; local_e <= (uVar3 & 0xff); local_e = local_e + 2) {
          if (*(char *)((int)*(undefined4 *)0xc1e0 + local_8) == 'u') {
            *(char *)0x661e = *(char *)0x661e + '\x01';
            *(int *)0x2072 = local_8;
          }
          local_8 = local_8 + 2;
        }
        local_8 = iVar7 + *(int *)0xc204;
      }
      if ('\0' < *(char *)0x661e) {
        FUN_1000_3a56();
        if (1 < *(byte *)(*(int *)0x2074 + 0x78d5)) {
          *(char *)0x661e = *(char *)0x661e << 1;
        }
        if ((int)(uint)*(byte *)(iVar4 + 0x24) < (int)*(char *)0x661e) {
          pcVar1 = (char *)((int)*(undefined4 *)(param_1 + 4) + 2);
          *pcVar1 = *pcVar1 + -1;
        }
        *(char *)(iVar4 + 0x24) = *(char *)(iVar4 + 0x24) - *(char *)0x661e;
        FUN_1000_5a75(param_1,0x2f);
      }
      if (*(char *)(iVar4 + 2) == -1) {
        FUN_1000_5bcc(&stack0xfffe);
      }
      if (*(char *)(param_1 + -0x21) == '\0') {
        *(int *)(param_1 + -0xe) = *(int *)(param_1 + -0xe) + 0x40;
      }
      if (((*(char *)(param_1 + -0x22) != '\0') && (*(int *)(param_1 + -0xe) < 0)) ||
         ((*(char *)(param_1 + -0x21) != '\0' && (0 < *(int *)(param_1 + -0xe))))) {
        *(int *)(param_1 + -0xe) = -(*(int *)(param_1 + -0xe) / 2);
      }
      if (((*(char *)(param_1 + -0x23) != '\0') && (*(int *)(param_1 + -0xc) < 0)) ||
         ((*(char *)(param_1 + -0x24) != '\0' && (0 < *(int *)(param_1 + -0xc))))) {
        *(int *)(param_1 + -0xc) = -(*(int *)(param_1 + -0xc) / 2);
      }
      return;
    }
    bVar2 = *(byte *)((int)*(undefined4 *)0xc1e0 + local_8);
    if ((bVar2 != 0) && (bVar2 < 0x4d)) {
      *(undefined1 *)(param_1 + -0x24) = 1;
      goto LAB_1000_5e59;
    }
    local_8 = local_8 + *(int *)0xc204;
    local_c = local_c + 1;
  } while( true );
}


