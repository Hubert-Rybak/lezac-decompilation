/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x10495 FUN_1000_0495
 * ev_1698e868c93583c167865b360ef1f211209cf41971c6df19035736795f4fc6d7
 */

void FUN_1000_0495(undefined4 param_1,undefined4 param_2,char param_3)

{
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  byte local_c;
  byte local_b;
  undefined1 local_a [4];
  undefined2 local_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x1000;
  local_6 = 0x4a0;
  FUN_1920_04df();
  FUN_1920_090e(3,&local_6,unaff_SS,(int)param_2,(int)((ulong)param_2 >> 0x10));
  FUN_1920_090e(3,local_a,unaff_SS,(int)param_1,(int)((ulong)param_1 >> 0x10));
  local_b = *(char *)0x79cb + 1;
  if ((*(char *)0x79cb != '\0') && (*(byte *)0x79cb != 0)) {
    local_c = 1;
    while( true ) {
      if (*(char *)(local_c + 0x79cd) == param_3) {
        local_b = local_c;
      }
      if (local_c == *(byte *)0x79cb) break;
      local_c = local_c + 1;
    }
  }
  if (local_b < 3) {
    if (*(byte *)0x79cb < local_b) {
      *(char *)0x79cb = *(char *)0x79cb + '\x01';
    }
    *(char *)(local_b + 0x79cd) = param_3;
    FUN_1920_090e(3,(uint)local_b * 3 + 0x79cf,unaff_DS,&local_6,unaff_SS);
    FUN_1920_090e(3,(uint)local_b * 3 + 0x79d9,unaff_DS,local_a,unaff_SS);
  }
  return;
}


