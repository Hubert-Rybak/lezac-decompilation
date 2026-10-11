/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1136e FUN_1000_136e
 * ev_c83fa127085d61495e56f82561734daa554bb26670d0970c26e1e898586dd61d
 */

void FUN_1000_136e(byte param_1,byte param_2,char param_3,undefined1 param_4,undefined4 param_5,
                  int param_6,int param_7,int param_8,int param_9)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  undefined1 uVar4;
  undefined1 extraout_AH;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  int local_108;
  uint local_104;
  byte local_102 [252];
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x1000;
  uStack_6 = 0x1379;
  FUN_1920_04df();
  FUN_1920_09f4(0xff,local_102,unaff_SS,(int)param_5,(int)((ulong)param_5 >> 0x10));
  *(undefined1 *)0x79a3 = 0;
  if (local_102[0] != 0) {
    local_104 = 1;
    while( true ) {
      bVar1 = local_102[local_104];
      iVar2 = (local_104 - 1) * (uint)param_2 + param_9;
      if (bVar1 != 0x20) {
        iVar3 = (bVar1 - 0x61) + (uint)param_1;
        local_108 = iVar3;
        if ((0x2f < bVar1) && (bVar1 < 0x40)) {
          iVar3 = param_1 + 0x1a;
          local_108 = (bVar1 - 0x30) + iVar3;
        }
        uVar4 = (undefined1)((uint)iVar3 >> 8);
        if (param_3 != '\0') {
          *(char *)0x661e = param_3;
          FUN_1000_12db(CONCAT11((char)((uint)(param_8 + param_6) >> 8),(undefined1)local_108),
                        param_8 + param_6,iVar2 + param_7);
          uVar4 = extraout_AH;
        }
        *(undefined1 *)0x661e = param_4;
        FUN_1000_12db(CONCAT11(uVar4,(undefined1)local_108),param_8,iVar2);
      }
      if (local_104 == local_102[0]) break;
      local_104 = local_104 + 1;
    }
  }
  return;
}


