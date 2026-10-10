/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x18ee2 FUN_18ac_0422
 * ev_c520ad816c2484216f3f67c053026126ebd380c9724c8998e793f3d2c554abd1
 */

void __stdcall16far FUN_18ac_0422(undefined4 param_1)

{
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  int local_186;
  undefined1 local_184;
  byte local_183;
  undefined1 local_182 [128];
  undefined1 local_102 [252];
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x18ac;
  uStack_6 = 0x8eed;
  FUN_1920_04df();
  FUN_1920_09f4(0xff,local_102,unaff_SS,(int)param_1,(int)((ulong)param_1 >> 0x10));
  FUN_1920_15ca(local_102,unaff_SS,local_182,unaff_SS);
  FUN_1920_15f8(1,local_182,unaff_SS);
  FUN_1920_16e3(&local_186,unaff_SS,1,&local_184,unaff_SS,local_182,unaff_SS);
  FUN_1920_16e3(&local_186,unaff_SS,1,&local_183,unaff_SS,local_182,unaff_SS);
  FUN_1920_16e3(&local_186,unaff_SS,(uint)local_183 << 6,0x7f74,unaff_DS,local_182,unaff_SS);
  local_186 = 0;
  while( true ) {
    if (*(char *)(local_186 + 0x7f74) == '\0') {
      *(undefined1 *)(local_186 + -0x5f56) = 0xff;
    }
    else {
      *(undefined1 *)(local_186 + -0x5f56) = 0;
    }
    if (local_186 == (uint)local_183 << 6) break;
    local_186 = local_186 + 1;
  }
  FUN_1920_1679(local_182,unaff_SS);
  return;
}


