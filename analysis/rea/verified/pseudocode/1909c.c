/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1909c FUN_18ac_05dc
 * ev_d2f72722d6ae32bae46817d108a0aaa62cfc35d25b928673104920f0998f569d
 */

void __stdcall16far FUN_18ac_05dc(undefined4 param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 local_18c [2];
  int local_18a;
  byte local_187;
  byte local_186;
  byte local_185;
  uint local_184;
  undefined1 local_182 [128];
  undefined1 local_102 [252];
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x18ac;
  uStack_6 = 0x90a7;
  FUN_1920_04df();
  FUN_1920_09f4(0xff,local_102,unaff_SS,(int)param_1,(int)((ulong)param_1 >> 0x10));
  FUN_1920_15ca(local_102,unaff_SS,local_182,unaff_SS);
  FUN_1920_15f8(1,local_182,unaff_SS);
  local_18a = 0;
  FUN_1920_16e3(local_18c,unaff_SS,1,&local_185,unaff_SS,local_182,unaff_SS);
  if (local_185 != 0) {
    local_184 = 1;
    while( true ) {
      FUN_1920_16e3(local_18c,unaff_SS,1,&local_186,unaff_SS,local_182,unaff_SS);
      FUN_1920_16e3(local_18c,unaff_SS,1,&local_187,unaff_SS,local_182,unaff_SS);
      *(byte *)(local_184 * 4 + -0x3cde) = local_186;
      *(byte *)(local_184 * 4 + -0x3cdd) = local_187;
      uVar2 = (uint)local_187;
      uVar1 = (uint)local_186;
      FUN_1920_16e3(local_18c,unaff_SS,uVar1 * uVar2,(int)*(undefined4 *)0xc1e4 + local_18a,
                    (int)((ulong)*(undefined4 *)0xc1e4 >> 0x10),local_182,unaff_SS);
      *(int *)(local_184 * 4 + -0x3cdc) = (int)*(undefined4 *)0xc1e4 + local_18a;
      local_18a = local_18a + uVar1 * uVar2;
      if (local_184 == local_185) break;
      local_184 = local_184 + 1;
    }
  }
  FUN_1920_1679(local_182,unaff_SS);
  return;
}


