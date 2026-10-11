/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x12597 FUN_1000_2597
 * ev_ef7e71b592dc5ceab1f304b1039347f6f0f8c89bb823ecf100c4bedb838f9367
 */

void FUN_1000_2597(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  byte local_b1;
  undefined1 local_b0 [128];
  undefined1 local_30 [42];
  undefined2 uStack_6;
  uint local_4;
  
  local_4 = 0x1000;
  uStack_6 = 0x25a2;
  FUN_1920_04df();
  *(undefined1 *)0x79c9 = *(undefined1 *)0x7f5c;
  *(undefined1 *)0x7f5c = 0;
  FUN_1920_142f();
  FUN_183f_0000(local_30,unaff_SS,0x20,0x2585,0x1920);
  *(undefined1 *)0x208c = 0;
  if (*(int *)0x7f5a == 0) {
    FUN_1920_15ca(0x2585,0x183f,local_b0,unaff_SS);
    FUN_1920_15f8(1,local_b0,unaff_SS);
    FUN_1920_16e3(0,0,1,&local_b1,unaff_SS,local_b0,unaff_SS);
    if (local_b1 != 0) {
      local_4 = 1;
      while( true ) {
        iVar1 = local_4 * 0xd;
        FUN_1920_16e3(0,0,4,iVar1 + 0x1af7,unaff_DS,local_b0,unaff_SS);
        FUN_1920_16e3(0,0,9,iVar1 + 0x1afb,unaff_DS,local_b0,unaff_SS);
        if (local_4 == local_b1) break;
        local_4 = local_4 + 1;
      }
    }
    FUN_1920_1679(local_b0,unaff_SS);
    iVar1 = FUN_1920_04a2();
    if (iVar1 != 0) {
      FUN_1000_00a3(1);
    }
  }
  else {
    local_4 = 1;
    uVar2 = 0x183f;
    while( true ) {
      iVar1 = local_4 * 0xd;
      FUN_1920_09f4(8,iVar1 + 0x1afb,unaff_DS,0x258e,uVar2);
      *(undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar1 + 0x1af7)) = 10000;
      *(undefined2 *)(iVar1 + 0x1af9) = 0;
      if (local_4 == 7) break;
      local_4 = local_4 + 1;
      uVar2 = 0x1920;
    }
    FUN_1000_16ca();
  }
  return;
}


