/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x15872 FUN_1000_5872
 * ev_3364fadfb108df8fd9e2c62701e17af090700f235f6c0d419e96e85a64dfc9b3
 */

void FUN_1000_5872(int param_1,byte param_2)

{
  uint uVar1;
  int iVar2;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 local_4;
  
  FUN_1920_04df();
  local_4 = 1;
  if (0x80 < param_2) {
    param_2 = param_2 + 0x80;
    local_4 = -1;
  }
  iVar2 = (uint)param_2 * 0x10;
  if (*(char *)(iVar2 + 0x79ed) == -1) {
    *(undefined2 *)(param_1 + -0x2c) = *(undefined2 *)(iVar2 + 0x79f5);
    *(undefined2 *)(param_1 + -0x2e) = *(undefined2 *)(iVar2 + 0x79f7);
    *(undefined2 *)(param_1 + -0xc) = 0;
    *(undefined2 *)(param_1 + -0xe) = 0;
  }
  else {
    *(int *)(param_1 + -0xc) = *(int *)(param_1 + -0xc) + *(int *)(iVar2 + 0x79f5) * local_4;
    *(int *)(param_1 + -0xe) = *(int *)(param_1 + -0xe) + *(int *)(iVar2 + 0x79f7) * local_4;
    uVar1 = (int)*(uint *)(param_1 + -0xc) >> 0xf;
    if ((int)(uint)*(byte *)(iVar2 + 0x79ed) < (int)((*(uint *)(param_1 + -0xc) ^ uVar1) - uVar1)) {
      if (*(int *)(param_1 + -0xc) < 1) {
        *(int *)(param_1 + -0xc) = *(int *)(param_1 + -0xc) + (uint)*(byte *)(iVar2 + 0x79ed);
      }
      else {
        *(int *)(param_1 + -0xc) = *(int *)(param_1 + -0xc) - (uint)*(byte *)(iVar2 + 0x79ed);
      }
    }
    uVar1 = (int)*(uint *)(param_1 + -0xe) >> 0xf;
    if ((int)(uint)*(byte *)(iVar2 + 0x79ed) < (int)((*(uint *)(param_1 + -0xe) ^ uVar1) - uVar1)) {
      if (*(int *)(param_1 + -0xe) < 1) {
        *(int *)(param_1 + -0xe) = *(int *)(param_1 + -0xe) + (uint)*(byte *)(iVar2 + 0x79ed);
      }
      else {
        *(int *)(param_1 + -0xe) = *(int *)(param_1 + -0xe) - (uint)*(byte *)(iVar2 + 0x79ed);
      }
    }
  }
  return;
}


