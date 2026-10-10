/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x13bb2 FUN_1000_3bb2
 * ev_c34ea270c55a4b1bb15d20abc18336b6a9e6771428abd332a03a1d848f0ed3ca
 */

void FUN_1000_3bb2(char *param_1,byte param_2)

{
  int iVar1;
  uint uVar2;
  char cVar3;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  bool bVar4;
  char local_e;
  byte local_d;
  int local_c;
  uint local_a;
  uint local_8;
  uint local_6;
  int local_4;
  
  local_4 = 0x1000;
  local_6 = 0x3bbd;
  FUN_1920_04df();
  local_a = (uint)param_2;
  local_6 = (uint)param_2 * (int)*param_1;
  local_4 = (int)local_6 >> 0xf;
  iVar1 = *(int *)0x2078;
  if (iVar1 != 0) {
    local_c = 1;
    while( true ) {
      local_8 = *(uint *)(local_c * 2 + 0x655c);
      if ((local_8 & 0x8000) == 0) {
        FUN_1000_370e(&local_e,unaff_SS,0,0,*(uint *)(local_c * 2 + 0x6598) >> 1);
        if (*(char *)0x79c8 == '\0') {
          return;
        }
        if (local_e == '\0') {
          *(undefined2 *)(local_c * 2 + 0x65d4) = *(undefined2 *)0x2080;
          local_a = *(byte *)(*(int *)0x2080 * 0xf + 0x661f) + local_a;
        }
        else {
          *(int *)(local_c * 2 + 0x65d4) = *(int *)0x207e + 20000;
          local_a = local_a + 1;
        }
      }
      else {
        *(uint *)0x2074 = local_8;
        FUN_1000_3a7e();
        if ((local_8 & 0x7fff) < 0x4000) {
          local_d = *(byte *)(*(int *)0x2074 * 0xf + 0x661f);
        }
        else {
          *(int *)0x2074 = *(int *)0x2074 + 20000;
          local_d = 1;
        }
        *(undefined2 *)(local_c * 2 + 0x65d4) = *(undefined2 *)0x2074;
        local_a = local_d + local_a;
        uVar2 = (uint)local_d * (int)*(char *)0x661e;
        bVar4 = CARRY2(uVar2,local_6);
        local_6 = uVar2 + local_6;
        local_4 = ((int)uVar2 >> 0xf) + local_4 + (uint)bVar4;
      }
      if (local_c == iVar1) break;
      local_c = local_c + 1;
    }
  }
  cVar3 = FUN_1920_0945();
  iVar1 = *(int *)0x2078;
  if (iVar1 != 0) {
    local_c = 1;
    while( true ) {
      uVar2 = *(uint *)(local_c * 2 + 0x65d4);
      if (uVar2 < 20000) {
        *(char *)(uVar2 * 0xf + 0x6617) = cVar3;
      }
      else {
        *(char *)((uVar2 + 0xb1e0) * 0xb + 0x2097) = cVar3;
      }
      if (local_c == iVar1) break;
      local_c = local_c + 1;
    }
  }
  *param_1 = cVar3;
  return;
}


