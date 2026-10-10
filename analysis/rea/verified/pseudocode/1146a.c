/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1146a FUN_1000_146a
 * ev_43a5feb9e00cd67587b15de4f0e9415ba873d2b2a741c4caba0d08494ef54ac2
 */

void FUN_1000_146a(char param_1,undefined2 param_2,undefined1 param_3,byte param_4,
                  undefined1 param_5,byte param_6,byte param_7,undefined4 param_8,undefined2 param_9
                  ,int param_10)

{
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 *puVar5;
  undefined2 uVar6;
  undefined1 *puVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined1 local_314 [252];
  undefined1 local_218 [2];
  undefined1 local_216 [2];
  undefined1 local_214 [252];
  int local_118;
  int local_116;
  undefined1 local_113;
  int local_112;
  int local_110;
  int local_10e;
  int local_10c;
  undefined1 local_10a [8];
  byte local_102 [252];
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x1000;
  uStack_6 = 0x1475;
  FUN_1920_04df();
  FUN_1920_09f4(0xff,local_102,unaff_SS,(int)param_8,(int)((ulong)param_8 >> 0x10));
  *(undefined1 *)0x79ba = 0;
  if (param_1 != '\0') {
    param_10 = 0xa0 - ((uint)local_102[0] * (uint)param_4 >> 1);
  }
  FUN_1920_09f4(6,local_10a,unaff_SS,0x1463,0x1920);
  local_112 = ((uint)param_6 - (uint)param_7) + 1;
  if (local_112 < 7) {
    local_10c = param_10 - (((uint)param_6 - (uint)param_7) + 2) * (uint)param_4;
    puVar5 = local_214;
    uVar10 = unaff_SS;
    FUN_1920_0a26(local_112,1,local_10a,unaff_SS);
    FUN_1920_0a67(local_102,unaff_SS);
    puVar7 = local_314;
    uVar6 = unaff_SS;
    FUN_1920_0a26(local_112,1,local_10a,unaff_SS);
    FUN_1920_0a67(puVar7,uVar6);
    FUN_1920_09f4(0xff,local_102,unaff_SS,puVar5,uVar10);
    local_116 = (uint)local_102[0] - local_112;
    if (0 < local_116) {
      local_10e = 1;
      while( true ) {
        puVar5 = local_216;
        uVar6 = unaff_SS;
        FUN_1920_0a26(local_112,local_10e,local_102,unaff_SS);
        FUN_1920_09f4(6,local_10a,unaff_SS,puVar5,uVar6);
        local_118 = local_112;
        if (0 < local_112) {
          local_110 = 1;
          while( true ) {
            if (local_110 == local_112) {
              local_113 = param_5;
            }
            else {
              local_113 = 0;
            }
            iVar3 = (uint)param_4 * local_110 + local_10c;
            uVar9 = 0xffff;
            uVar8 = 0xffff;
            puVar5 = local_218;
            uVar6 = unaff_SS;
            uVar10 = param_9;
            FUN_1920_0b0a(CONCAT11((char)((uint)iVar3 >> 8),local_10a[local_110]));
            iVar4 = ((uint)param_6 - local_110) + 1;
            uVar2 = (undefined1)((uint)iVar4 >> 8);
            FUN_1000_136e(CONCAT11(uVar2,param_3),CONCAT11(uVar2,param_4),CONCAT11(uVar2,local_113),
                          iVar4,puVar5,uVar6,uVar8,uVar9,uVar10,iVar3);
            if (local_110 == local_118) break;
            local_110 = local_110 + 1;
          }
        }
        FUN_184a_029c(param_2);
        cVar1 = FUN_184a_02fd();
        if (cVar1 != '\0') {
          param_2 = 0;
          uVar2 = FUN_184a_030f();
          *(undefined1 *)0x2058 = uVar2;
          *(undefined1 *)0x79ba = 2;
          if (*(char *)0x2058 == '\x1b') {
            *(undefined1 *)0x79ba = 1;
          }
        }
        local_10c = local_10c + (uint)param_4;
        if (local_10e == local_116) break;
        local_10e = local_10e + 1;
      }
    }
  }
  return;
}


