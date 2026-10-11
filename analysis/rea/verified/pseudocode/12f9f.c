/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x12f9f FUN_1000_2f9f
 * ev_bceafc40a21299c0367fdd202f86fb9e3e45217b3c3f359740b9a2d977fc8953
 */

void FUN_1000_2f9f(undefined1 param_1,undefined1 param_2,undefined1 param_3,int param_4,uint param_5
                  ,uint param_6,undefined2 param_7,undefined2 param_8)

{
  uint uVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 local_6;
  
  FUN_1920_04df();
  if (*(byte *)0x208d < 0x1e) {
    *(char *)0x208d = *(char *)0x208d + '\x01';
    *(undefined2 *)0x2072 = 1;
    uVar1 = (int)param_6 >> 0xf;
    if (0x7ff < (int)((param_6 ^ uVar1) - uVar1)) {
      param_6 = ((int)((param_6 ^ uVar1) - uVar1) / (int)param_6) * 0x7ff;
    }
    uVar1 = (int)param_5 >> 0xf;
    if (0x7ff < (int)((param_5 ^ uVar1) - uVar1)) {
      param_5 = ((int)((param_5 ^ uVar1) - uVar1) / (int)param_5) * 0x7ff;
    }
    iVar2 = (uint)*(byte *)0x208d * 0x26;
    _local_6 = (undefined1 *)CONCAT22(unaff_DS,(undefined1 *)(iVar2 + 0x1bae));
    *(undefined2 *)(iVar2 + 0x1bb8) = 0;
    *(undefined2 *)(iVar2 + 0x1bba) = 0;
    *(undefined1 *)(iVar2 + 0x1baf) = *(undefined1 *)0xc496;
    *(uint *)(iVar2 + 0x1bb4) = param_6;
    *(uint *)(iVar2 + 0x1bb6) = param_5;
    *_local_6 = param_3;
    *(undefined1 *)(iVar2 + 0x1bb0) = param_2;
    *(undefined1 *)(iVar2 + 0x1bc3) = param_1;
    if (param_4 == 0x1f) {
      *(undefined1 *)(iVar2 + 0x1bc2) = 0;
    }
    else {
      *(char *)(iVar2 + 0x1bc2) = '\x10' - *(char *)(param_4 * 4 + -0x3cdd);
    }
    FUN_18ac_0517(param_7,param_8,param_4);
  }
  else {
    *(undefined2 *)0x2072 = 0;
  }
  return;
}


