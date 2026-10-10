/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1326e FUN_1000_326e
 * ev_def7e51a3b91683f4510c2873e32dd568dbbb61a382e8c42a9a98d14a6eb7626
 */

void FUN_1000_326e(byte param_1,int param_2,int param_3)

{
  byte bVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 local_c;
  undefined1 local_4;
  
  FUN_1920_04df();
  bVar1 = *(byte *)(param_1 + 0x1b73);
  local_4 = *(byte *)((uint)param_1 * 4 + (uint)bVar1 + 0x1b67);
  if (1 < *(byte *)(param_1 + 0x1b75)) {
    FUN_1000_07c5(0x12,0x10,0x10,param_2,param_3);
    *(undefined1 *)0x79a3 = 0x12;
    iVar2 = (bVar1 + 0x39) * 4;
    _local_c = (byte *)CONCAT22(unaff_DS,(byte *)(iVar2 + -0x3cde));
    FUN_1000_12db(bVar1 + 0x39,(0x10 - *(byte *)(iVar2 + -0x3cdd) >> 1) + param_2,
                  (0x10 - *_local_c >> 1) + param_3);
  }
  *(undefined1 *)(param_1 + 0x1b75) = 0;
  if (99 < local_4) {
    local_4 = 99;
  }
  *(int *)0x2072 = (param_2 + 0x13) * 0x140 + param_3 + 8;
  FUN_1000_313d(2,local_4);
  return;
}


