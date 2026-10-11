/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x116ca FUN_1000_16ca
 * ev_786bb693117d83fd397a348ecf4250560ae751e87535f6f78675152a5fbedcb5
 */

void __cdecl16near FUN_1000_16ca(void)

{
  undefined1 uVar1;
  uint uVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 local_84;
  byte local_83;
  undefined1 local_82 [124];
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x1000;
  uVar4 = 0x1920;
  uStack_6 = 0x16d5;
  FUN_1920_04df();
  do {
    FUN_1920_15ca(0x167e,uVar4,local_82,unaff_SS);
    FUN_1920_1601(1,local_82,unaff_SS);
    local_84 = 7;
    FUN_1920_16ea(0,0,1,&local_84,unaff_SS,local_82,unaff_SS);
    local_83 = 1;
    while( true ) {
      uVar2 = (uint)local_83;
      FUN_1920_16ea(0,0,4,uVar2 * 0xd + 0x1af7,unaff_DS,local_82,unaff_SS);
      FUN_1920_16ea(0,0,9,uVar2 * 0xd + 0x1afb,unaff_DS,local_82,unaff_SS);
      if (local_83 == 7) break;
      local_83 = local_83 + 1;
    }
    FUN_1920_1679(local_82,unaff_SS);
    iVar3 = FUN_1920_04a2();
    if (iVar3 == 0) {
      return;
    }
    FUN_1000_07c5(0,200,0x140,0,0);
    FUN_1000_136e(0x1b,9,0xd,0x37,0x1687,0x1920,1,1,0x59,10);
    FUN_1000_136e(0x1b,9,0xd,0x37,0x169a,0x1920,1,1,100,10);
    FUN_1000_136e(0x1b,9,0xd,0x37,0x16ae,0x1920,1,1,0x6f,10);
    do {
      uVar4 = 0x184a;
      uVar1 = FUN_184a_030f();
      *(undefined1 *)0x2058 = uVar1;
      if (*(char *)0x2058 == '\x1b') break;
    } while (*(char *)0x2058 != '\r');
    if (*(char *)0x2058 != '\r') {
      return;
    }
  } while( true );
}


