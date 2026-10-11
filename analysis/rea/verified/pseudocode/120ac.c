/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x120ac FUN_1000_20ac
 * ev_b3dc8f507982df08191a9f9cd340a08555699bc0f1719f1dec48dd2507cfd77b
 */

void FUN_1000_20ac(void)

{
  undefined1 uVar1;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 *puVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 uVar5;
  undefined1 local_20a [256];
  undefined2 *local_10a;
  undefined1 local_106 [256];
  int local_6;
  int local_4;
  
  local_4 = 0x1000;
  local_6 = 0x20b7;
  FUN_1920_04df();
  FUN_1000_01fc();
  local_6 = 0x48;
  FUN_1000_146a(1,*(undefined2 *)0x78ce,0x1b,9,6,10,6,0x2095,0x1920,10,9);
  local_4 = 1;
  while( true ) {
    _local_10a = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(local_4 * 0xd + 0x1af7));
    FUN_1920_154e(0xff,local_106,unaff_SS,0,*_local_10a,*(undefined2 *)(local_4 * 0xd + 0x1af9));
    uVar5 = 9;
    puVar2 = local_20a;
    uVar3 = unaff_SS;
    iVar4 = local_6;
    FUN_1920_09da((undefined2 *)_local_10a + 2,(int)((ulong)_local_10a >> 0x10));
    FUN_1920_0a67(0x20a7,0x1920);
    FUN_1920_0a67(local_106,unaff_SS);
    FUN_1000_146a(1,*(undefined2 *)0x78ce,0x1b,9,6,10,6,puVar2,uVar3,iVar4,uVar5);
    local_6 = local_6 + 0xb;
    if (local_4 == 7) break;
    local_4 = local_4 + 1;
  }
  uVar1 = FUN_184a_030f();
  *(undefined1 *)0x2058 = uVar1;
  return;
}


