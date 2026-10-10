/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x11845 FUN_1000_1845
 * ev_33b4ce2315a6134893b3ade8bef79fd94143f32122f899fab5fc4abf0fb21f57
 */

void FUN_1000_1845(undefined2 param_1,int param_2,uint param_3,uint param_4)

{
  char cVar1;
  byte bVar2;
  undefined1 uVar3;
  undefined1 extraout_AH;
  uint uVar4;
  int iVar5;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 *puVar6;
  undefined2 uVar7;
  undefined1 *puVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  undefined1 local_212 [256];
  undefined1 local_112 [252];
  uint *local_16;
  undefined1 local_12 [10];
  undefined1 local_8 [2];
  undefined2 uStack_6;
  undefined2 local_4;
  
  local_4 = 0x1000;
  uStack_6 = 0x1850;
  FUN_1920_04df();
  FUN_1000_01fc();
  *(undefined2 *)0x2074 = 0x78;
  *(undefined1 *)0x799f = 0xb;
  FUN_1000_165a();
  for (local_4._1_1_ = 7; local_4._1_1_ != 0; local_4._1_1_ = local_4._1_1_ - 1) {
    iVar5 = *(int *)((uint)local_4._1_1_ * 0xd + 0x1af9);
    if (((int)param_4 <= iVar5) &&
       (((int)param_4 < iVar5 || (param_3 <= *(uint *)((uint)local_4._1_1_ * 0xd + 0x1af7)))))
    break;
  }
  local_4._1_1_ = local_4._1_1_ + 1;
  FUN_1920_154e(2,local_8,unaff_SS,0,local_4._1_1_,0);
  uVar11 = 0x3c;
  uVar9 = 10;
  puVar8 = local_212;
  uVar10 = unaff_SS;
  FUN_1920_09da(0x17f2,0x1920);
  puVar6 = local_112;
  uVar7 = unaff_SS;
  FUN_1920_0b0a(CONCAT11(extraout_AH,*(undefined1 *)(param_2 + 0x1abe)));
  FUN_1920_0a67(puVar6,uVar7);
  FUN_1000_146a(1,*(undefined2 *)0x78ce,0x1b,0xb,0x19,0x1f,0x1b,puVar8,uVar10,uVar9,uVar11);
  uVar9 = 0x3c;
  uVar10 = 0x15;
  puVar8 = local_112;
  uVar7 = unaff_SS;
  FUN_1920_09da(0x17fd,0x1920);
  FUN_1920_0a67(local_8,unaff_SS);
  FUN_1920_0a67(0x180e,0x1920);
  FUN_1000_146a(1,*(undefined2 *)0x78ce,0x1b,0xb,0x19,0x1f,0x1b,puVar8,uVar7,uVar10,uVar9);
  FUN_1000_146a(1,*(undefined2 *)0x78ce,0x1b,0xb,0x19,0x1f,0x1b,0x1815,0x1920,0x20,0x3c);
  FUN_1000_146a(1,*(undefined2 *)0x78ce,0x1b,0xb,0x19,0x1f,0x1b,0x1825,0x1920,0x2b,0x50);
  FUN_1000_07fa(8,7,0xd,0x4c,0x3a,0x7a);
  FUN_1920_09f4(8,local_12,unaff_SS,0x183c,0x1920);
  local_4 = CONCAT11(local_4._1_1_,1);
  do {
    FUN_1000_136e(0x1b,9,0x37,0x4d,local_12,unaff_SS,1,1,0x3c,0x7c);
    uVar3 = FUN_184a_030f();
    *(undefined1 *)0x2058 = uVar3;
    uVar3 = FUN_1920_1807(*(undefined1 *)0x2058);
    *(undefined1 *)0x2058 = uVar3;
    if ((((*(byte *)0x2058 < 0x41) || (0x5a < *(byte *)0x2058)) && (*(char *)0x2058 != ' ')) ||
       (8 < (byte)local_4)) {
      if ((*(char *)0x2058 == '\b') && (1 < (byte)local_4)) {
        (&stack0xffed)[local_4 & 0xff] = 0x3a;
        bVar2 = (byte)local_4 - 1;
        local_4 = CONCAT11(local_4._1_1_,bVar2);
        FUN_1000_07c5(8,10,10,0x3c,(uint)bVar2 * 9 + 0x73);
      }
    }
    else {
      *(byte *)0x2058 = *(byte *)0x2058 | 0x20;
      local_12[local_4 & 0xff] = *(undefined1 *)0x2058;
      FUN_1000_07c5(8,10,10,0x3c,(local_4 & 0xff) * 9 + 0x73);
      cVar1 = (byte)local_4 + '\x01';
      local_4 = CONCAT11(local_4._1_1_,cVar1);
      *(undefined2 *)0x2074 = 0;
      *(char *)0x799f = cVar1;
      FUN_1000_165a();
    }
  } while (*(char *)0x2058 != '\r');
  *(undefined2 *)0x2074 = 8;
  *(undefined1 *)0x799f = 0xb;
  FUN_1000_165a();
  if ((local_4._1_1_ < 7) && (uVar4 = (uint)local_4._1_1_, uVar4 + 1 < 8)) {
    *(undefined2 *)0x2082 = 7;
    while (FUN_1920_090e(0xd,*(int *)0x2082 * 0xd + 0x1af7,unaff_DS,
                         (*(int *)0x2082 + -1) * 0xd + 0x1af7,unaff_DS),
          *(uint *)0x2082 != uVar4 + 1) {
      *(int *)0x2082 = *(int *)0x2082 + -1;
    }
  }
  iVar5 = (local_4 >> 8) * 0xd;
  _local_16 = (uint *)CONCAT22(unaff_DS,(uint *)(iVar5 + 0x1af7));
  FUN_1920_09f4(8,iVar5 + 0x1afb,unaff_DS,local_12,unaff_SS);
  *_local_16 = param_3;
  ((uint *)_local_16)[1] = param_4;
  FUN_1000_16ca();
  return;
}


