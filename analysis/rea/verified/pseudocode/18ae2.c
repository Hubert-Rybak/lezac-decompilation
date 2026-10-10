/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x18ae2 FUN_18ac_0022
 * ev_3995794e5e32b61736bac762830e5fb7e0ce5d8d3d1da70ecbe77f6e7e993f87
 */

void __cdecl16far FUN_18ac_0022(void)

{
  undefined2 unaff_DS;
  byte local_3;
  
  FUN_1920_04df();
  local_3 = 0xb0;
  while( true ) {
    FUN_18ac_0000((int)((local_3 - 0xb0) * -0xc) / 0x26 + 0xe,(int)((local_3 - 0xb0) * 0x17) / 0x26,
                  (int)((local_3 - 0xb0) * 0x2b) / 0x26,local_3);
    if (local_3 == 0xd6) break;
    local_3 = local_3 + 1;
  }
  *(undefined2 *)0xc1e8 = 0;
  while (*(char *)((int)*(undefined4 *)0xc498 + *(int *)0xc1e8) =
              (char)(*(uint *)0xc1e8 / (uint)(*(int *)0xc1ec << 2)) + -0x50,
        *(int *)0xc1e8 != -0x15a1) {
    *(int *)0xc1e8 = *(int *)0xc1e8 + 1;
  }
  return;
}


