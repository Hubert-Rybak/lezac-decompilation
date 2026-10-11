/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x101fc FUN_1000_01fc
 * ev_a16415cc0737a49ce0e4910a1a5bb97d3fff7bf917704ea8d9d065cd5c8d263b
 */

void __cdecl16near FUN_1000_01fc(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint local_10;
  char *local_e;
  uint local_c;
  int local_4;
  
  FUN_1920_04df();
  FUN_1000_07c5(0,200,0x140,0,0);
  local_e = (char *)0x0;
  local_10 = 0;
  local_4 = 0;
  iVar1 = FUN_1920_13a8(0x50);
  iVar2 = FUN_1920_13a8(0x50);
  FUN_1000_0139(&stack0xfffe);
  local_c = 0;
  while( true ) {
    local_4 = local_4 + iVar1 + 1;
    if (100 < local_4) {
      local_4 = local_4 + -100;
      local_10 = local_10 + 1;
    }
    if (local_c % 0x140 == 0) {
      FUN_1920_0f0f();
      FUN_1920_0efb();
      FUN_1920_0f13();
      FUN_1920_0945();
      FUN_1920_0efb();
      iVar3 = FUN_1920_0f13();
      local_4 = local_4 + iVar2 + 1 + iVar3;
      if (100 < local_4) {
        local_4 = local_4 + -100;
        local_10 = local_10 + 1;
      }
    }
    *local_e = (char)((ulong)local_10 % 7) + -0x50;
    local_e = local_e + 1;
    if (local_c == 63999) break;
    local_c = local_c + 1;
  }
  return;
}


