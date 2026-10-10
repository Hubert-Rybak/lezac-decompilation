/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x11d61 FUN_1000_1d61
 * ev_7c5e03fd3059a5080d7a42f9e00e31822bcce157624b4190f51f4477eb9148d0
 */

void __cdecl16near FUN_1000_1d61(void)

{
  uint *puVar1;
  char cVar2;
  undefined1 uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  undefined2 uVar8;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 *puVar9;
  undefined2 uVar10;
  undefined1 local_318 [256];
  int local_218;
  uint *local_216;
  undefined1 local_20a [256];
  undefined1 local_10a [258];
  undefined2 local_8;
  int local_6;
  int local_4;
  
  local_4 = 0x1000;
  local_6 = 0x1d6c;
  FUN_1920_04df();
  if (*(char *)0x79f8 != '\x01') {
    FUN_18ac_05dc(0x1d45,0x1920);
    iVar4 = FUN_1920_04a2();
    if (iVar4 != 0) {
      FUN_1000_00a3(1);
    }
    *(undefined1 *)0x79f8 = 1;
  }
  FUN_184a_029c(500);
  *(undefined2 *)0x2074 = 0x3d;
  *(undefined1 *)0x799f = 10;
  FUN_1000_165a();
  while (cVar2 = FUN_184a_02fd(), cVar2 != '\0') {
    uVar3 = FUN_184a_030f();
    *(undefined1 *)0x2058 = uVar3;
  }
  local_4 = 0x3c;
  FUN_18ac_0000(0x1f,0x1f,0x1f,0xffff);
  uVar5 = *(int *)0x78c8 * 10;
  FUN_1000_146a(1,*(undefined2 *)0x78ce,1,0xb,0x19,0x1f,0x1b,(uint)*(byte *)0x79cc * 0xd00 + 0xc8a,
                unaff_DS,local_4,0x3c);
  local_4 = local_4 + 0x15;
  FUN_1920_154e(0xff,local_20a,unaff_SS,0,uVar5,0);
  uVar10 = 0x3c;
  puVar9 = local_318;
  uVar8 = unaff_SS;
  iVar4 = local_4;
  FUN_1920_09da((uint)*(byte *)0x79cc * 0xd00 + 0xb8a,unaff_DS);
  FUN_1920_0a67(local_20a,unaff_SS);
  FUN_1000_146a(1,*(undefined2 *)0x78ce,0x1b,9,0xfff1,0xfff4,0xfff0,puVar9,uVar8,iVar4,uVar10);
  local_4 = local_4 + 0x12;
  FUN_1000_146a(1,*(undefined2 *)0x78ce,0x1b,9,0x19,0xfff4,0xfff0,0x1d4f,0x1920,local_4,0x3c);
  local_4 = local_4 + 0x15;
  local_6 = 1;
  while( true ) {
    if (*(char *)(local_6 + 0x79e5) == '\x01') {
      if (local_6 == 1) {
        local_216 = (uint *)0x785a;
        local_8 = 0;
      }
      else {
        local_216 = (uint *)0x7888;
        local_8 = *(undefined2 *)0x2084;
      }
      _local_216 = (uint *)CONCAT22(unaff_DS,local_216);
      FUN_1920_154e(0xff,local_10a,unaff_SS,0,local_6,local_6 >> 0xf);
      puVar9 = local_318;
      uVar8 = unaff_SS;
      FUN_1920_09da((uint)*(byte *)0x79cc * 0xd00 + 0xa8a,unaff_DS);
      FUN_1920_0a67(0x1d5b,0x1920);
      FUN_1920_0a67(local_10a,unaff_SS);
      FUN_1920_0a67(0x1d5d,0x1920);
      FUN_1920_09f4(0xff,local_10a,unaff_SS,puVar9,uVar8);
      uVar6 = (uint)*(byte *)(local_6 * 4 + 0x1b69) * 100 +
              (uint)*(byte *)(local_6 * 4 + 0x1b6a) * 500 +
              (uint)*(byte *)(local_6 * 4 + 0x1b6b) * 2000;
      FUN_1920_154e(0xff,local_20a,unaff_SS,0,uVar6,(int)uVar6 >> 0xf);
      puVar9 = local_318;
      uVar8 = unaff_SS;
      FUN_1920_09da(local_10a,unaff_SS);
      FUN_1920_0a67(local_20a,unaff_SS);
      FUN_1920_09f4(0xff,local_10a,unaff_SS,puVar9,uVar8);
      FUN_1000_146a(1,*(undefined2 *)0x78ce,0x1b,9,0xd,0x1f,0x1b,local_10a,unaff_SS,local_4,0x3c);
      local_4 = local_4 + 0xb;
      local_218 = *(int *)(local_6 * 2 + 0x79f2) + -0x2400;
      uVar8 = (undefined2)((ulong)_local_216 >> 0x10);
      uVar7 = *_local_216;
      *_local_216 = *_local_216 + uVar6 + uVar5;
      puVar1 = (uint *)_local_216 + 1;
      *puVar1 = *puVar1 + ((int)uVar6 >> 0xf) + (uint)CARRY2(uVar6,uVar5) +
                (uint)CARRY2(uVar7,uVar6 + uVar5);
      FUN_1000_11b0((uint *)_local_216,uVar8);
      while ((byte)((uint *)_local_216)[0x16] < 2) {
        FUN_1000_1287(local_8);
        *(int *)0x2074 = local_218;
        FUN_1000_1239(local_8);
        FUN_184a_029c(0xf);
        uVar7 = FUN_1920_13a8(4);
        if (2 < uVar7) {
          *(undefined2 *)0x2074 = 0x21;
          FUN_1000_165a();
        }
      }
      FUN_184a_029c(200);
    }
    if (local_6 == 2) break;
    local_6 = local_6 + 1;
  }
  uVar3 = FUN_184a_030f();
  *(undefined1 *)0x2058 = uVar3;
  *(char *)0x79b7 = *(char *)0x79b7 + '\x01';
  local_6 = 1;
  while( true ) {
    if (*(char *)(local_6 + 0x79e5) == '\x02') {
      *(undefined1 *)(local_6 + 0x79e5) = 1;
    }
    if (local_6 == 2) break;
    local_6 = local_6 + 1;
  }
  return;
}


