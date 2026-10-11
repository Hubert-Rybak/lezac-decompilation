/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1030b FUN_1000_030b
 * ev_4c63345759debcea73303dd7a604fc1c322cec1cce45206d1272f565139b1e1d
 */

void FUN_1000_030b(undefined4 param_1)

{
  code *pcVar1;
  char cVar2;
  undefined1 uVar3;
  int iVar4;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  int local_798;
  int local_796;
  undefined2 local_794;
  undefined2 uStack_792;
  byte *local_790;
  byte local_78c [770];
  byte local_48a [770];
  undefined2 local_188;
  undefined2 local_186;
  undefined2 local_184;
  undefined1 local_182 [128];
  undefined1 local_102 [252];
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x1000;
  uStack_6 = 0x316;
  FUN_1920_04df();
  FUN_1920_09f4(0xff,local_102,unaff_SS,(int)param_1,(int)((ulong)param_1 >> 0x10));
  FUN_1920_15ca(local_102,unaff_SS,local_182,unaff_SS);
  FUN_1920_15f8(1,local_182,unaff_SS);
  FUN_1920_16e3(0,0,0x300,local_48a,unaff_SS,local_182,unaff_SS);
  local_794 = *(undefined2 *)0x7f70;
  uStack_792 = *(undefined2 *)0x7f72;
  FUN_1920_16e3(0,0,2,&local_188,unaff_SS,local_182,unaff_SS);
  FUN_1920_16e3(0,0,local_188,local_794,uStack_792,local_182,unaff_SS);
  FUN_1920_1679(local_182,unaff_SS);
  iVar4 = FUN_1920_04a2();
  if (iVar4 != 0) {
    FUN_1000_00a3(1);
  }
  local_186 = 0;
  local_184 = 0xa000;
  local_796 = 0;
  while( true ) {
    cVar2 = FUN_184a_02fd();
    if ((cVar2 != '\0') && (2 < local_796)) {
      uVar3 = FUN_184a_030f();
      *(undefined1 *)0x2058 = uVar3;
      local_796 = 0x3f;
    }
    for (local_798 = 0;
        local_78c[local_798] = (byte)((int)((uint)local_48a[local_798] * local_796) / 0x3f),
        local_798 != 0x300; local_798 = local_798 + 1) {
    }
    for (local_798 = 0x1e; local_78c[local_798] = local_48a[local_798], local_798 != 0x20;
        local_798 = local_798 + 1) {
    }
    local_790 = local_78c;
    pcVar1 = (code *)swi(0x10);
    (*pcVar1)();
    if (local_796 == 0) {
      FUN_182d_0000(64000,&local_186,unaff_SS,0x16,&local_794,unaff_SS);
    }
    FUN_184a_029c(0x16);
    if (local_796 == 0x3f) break;
    local_796 = local_796 + 1;
  }
  return;
}


