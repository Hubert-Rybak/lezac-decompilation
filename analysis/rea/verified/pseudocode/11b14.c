/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x11b14 FUN_1000_1b14
 * ev_58ae62fa24929ac0b508314ef77898587ac7b21d5423ef40968dd02f19ce4d53
 */

void FUN_1000_1b14(char param_1)

{
  uint uVar1;
  uint *puVar2;
  undefined2 *puVar3;
  undefined1 uVar4;
  int iVar5;
  undefined1 extraout_AH;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 *puVar6;
  undefined2 uVar7;
  undefined1 *puVar8;
  undefined2 uVar9;
  undefined1 local_30e [256];
  undefined1 local_20e [252];
  uint auStack_112 [6];
  undefined1 local_106 [256];
  int local_6;
  int local_4;
  
  local_4 = 0x1000;
  local_6 = 0x1b1f;
  FUN_1920_04df();
  FUN_1000_01fc();
  FUN_1920_1679(0x7eda,unaff_DS);
  FUN_1920_04a9();
  if (*(char *)0x79f8 != '\x01') {
    FUN_18ac_05dc(0x1ad7,0x1920);
    iVar5 = FUN_1920_04a2();
    if (iVar5 != 0) {
      FUN_1000_00a3(1);
    }
    *(undefined1 *)0x79f8 = 1;
  }
  local_4 = 0x4d;
  *(undefined1 *)0x208c = 0;
  if (param_1 == '\x01') {
    FUN_1000_146a(1,*(undefined2 *)0x78ce,1,0xb,0x19,0x1f,0x1b,0x1ae1,0x1920,0x4d,0x3c);
  }
  else if (param_1 == '\x02') {
    FUN_1000_146a(1,*(undefined2 *)0x78ce,0x1b,0xb,0x19,0x1f,0x1b,0x1aeb,0x1920,0x37,0x3c);
    FUN_1000_146a(1,*(undefined2 *)0x78ce,1,0xb,0x19,0x1f,0x1b,0x1af9,0x1920,0x42,0x3c);
    *(undefined1 *)0x208c = 1;
  }
  local_4 = local_4 + 0x14;
  FUN_1000_146a(1,*(undefined2 *)0x78ce,1,0xb,0x19,0xfff4,0xfff0,
                (uint)*(byte *)0x79cc * 0xd00 + 0x98a,unaff_DS,local_4,0x3c);
  local_4 = local_4 + 0x18;
  local_6 = 1;
  while( true ) {
    iVar5 = local_6;
    if (local_6 == 1) {
      *(undefined2 *)0x78b6 = 0x785a;
      *(undefined2 *)0x78b8 = unaff_DS;
      *(undefined1 *)0x2058 = 0x31;
    }
    else {
      *(undefined2 *)0x78b6 = 0x7888;
      *(undefined2 *)0x78b8 = unaff_DS;
      *(undefined1 *)0x2058 = 0x32;
    }
    puVar2 = (uint *)*(undefined4 *)0x78b6;
    uVar1 = ((uint *)puVar2)[1];
    auStack_112[local_6 * 2] = *puVar2;
    auStack_112[iVar5 * 2 + 1] = uVar1;
    if ((0 < (int)auStack_112[local_6 * 2 + 1]) ||
       ((-1 < (int)auStack_112[local_6 * 2 + 1] && (auStack_112[local_6 * 2] != 0)))) {
      puVar3 = (undefined2 *)*(undefined4 *)0x78b6;
      FUN_1920_154e(0xff,local_106,unaff_SS,0,*puVar3,((undefined2 *)puVar3)[1]);
      puVar8 = local_30e;
      uVar9 = unaff_SS;
      FUN_1920_09da((uint)*(byte *)0x79cc * 0xd00 + 0xa8a,unaff_DS);
      puVar6 = local_20e;
      uVar7 = unaff_SS;
      FUN_1920_0b0a(CONCAT11(extraout_AH,*(undefined1 *)0x2058));
      FUN_1920_0a67(puVar6,uVar7);
      FUN_1920_0a67(0x1b11,0x1920);
      FUN_1920_0a67(local_106,unaff_SS);
      FUN_1920_09f4(0xff,local_106,unaff_SS,puVar8,uVar9);
      FUN_1000_146a(1,*(undefined2 *)0x78ce,0x1b,9,0x19,0xfff4,0xfff0,local_106,unaff_SS,local_4,
                    0x3c);
      local_4 = local_4 + 0xb;
    }
    if (local_6 == 2) break;
    local_6 = local_6 + 1;
  }
  uVar4 = FUN_184a_030f();
  *(undefined1 *)0x2058 = uVar4;
  local_6 = 1;
  while( true ) {
    if ((*(int *)0x1b54 < (int)auStack_112[local_6 * 2 + 1]) ||
       ((*(int *)0x1b54 <= (int)auStack_112[local_6 * 2 + 1] &&
        (*(uint *)0x1b52 <= auStack_112[local_6 * 2])))) {
      FUN_1000_1845(&stack0xfffe,local_6,auStack_112[local_6 * 2],auStack_112[local_6 * 2 + 1]);
    }
    if (local_6 == 2) break;
    local_6 = local_6 + 1;
  }
  return;
}


