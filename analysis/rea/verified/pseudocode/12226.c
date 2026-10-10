/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x12226 FUN_1000_2226
 * ev_b1ee02cf2cd9a03b3f253c52b7115bc066d9e163446b3c8ab8851cb6fc323fa5
 */

void FUN_1000_2226(undefined2 param_1,char param_2,char param_3,int param_4,byte param_5,
                  byte param_6)

{
  undefined1 uVar1;
  int iVar2;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 *puVar3;
  undefined2 uVar4;
  undefined1 local_30a [256];
  char local_20a;
  undefined1 local_209;
  int local_208;
  undefined2 local_206 [2];
  undefined1 local_202 [256];
  undefined1 local_102 [252];
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x1000;
  uStack_6 = 0x2231;
  FUN_1920_04df();
  local_209 = *(undefined1 *)0x78ce;
  if (param_3 != '\0') {
    FUN_1000_01fc();
  }
  local_208 = param_4;
  *(uint *)0x2082 = (uint)param_6;
  do {
    if ((uint)param_5 < *(uint *)0x2082) {
      if (param_3 != '\0') {
        uVar1 = FUN_184a_030f();
        *(undefined1 *)0x2058 = uVar1;
      }
      return;
    }
    iVar2 = (uint)*(byte *)0x79cc * 0xd00 + *(int *)0x2082 * 0x100 + -0x76;
    uVar1 = iVar2 == 0;
    FUN_1920_09f4(0xff,local_102,unaff_SS,iVar2,unaff_DS);
    local_206[0] = 1;
    while( true ) {
      puVar3 = local_30a;
      uVar4 = unaff_SS;
      FUN_1000_216c(param_1,&local_20a,unaff_SS,local_206,unaff_SS,local_102,unaff_SS);
      FUN_1920_09f4(0xff,local_202,unaff_SS,puVar3,uVar4);
      FUN_1920_0adf(0x2224,0x1920,local_202,unaff_SS);
      if ((bool)uVar1) {
        uVar1 = FUN_184a_030f();
        *(undefined1 *)0x2058 = uVar1;
        FUN_1000_01fc();
        local_208 = 0x14;
      }
      else {
        FUN_1000_146a(1,local_209,0x1b,9,6,10,6,local_202,unaff_SS,local_208,9);
        local_208 = local_208 + 10;
        if ((*(char *)0x79ba == '\x02') && (param_2 != '\0')) {
          local_209 = 0;
        }
        if ((*(char *)0x79ba == '\x01') && (param_3 != '\0')) {
          return;
        }
      }
      if (local_20a != '\0') break;
      uVar1 = 1;
    }
    *(int *)0x2082 = *(int *)0x2082 + 1;
  } while( true );
}


