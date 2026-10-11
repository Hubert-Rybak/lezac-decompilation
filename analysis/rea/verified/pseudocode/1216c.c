/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1216c FUN_1000_216c
 * ev_cf29cb609d4b81b8ddc9b4ddebd267fcc6c105a3c4cb73ba8a62929d2650a947
 */

void FUN_1000_216c(undefined2 param_1,undefined1 *param_2,int *param_3,byte *param_4,
                  undefined4 param_5)

{
  undefined2 uVar1;
  undefined2 unaff_SS;
  undefined1 *puVar2;
  undefined2 uVar3;
  undefined1 *puVar4;
  undefined2 uVar5;
  undefined1 local_304 [256];
  undefined1 local_204 [258];
  undefined1 local_102 [252];
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x1000;
  uStack_6 = 0x2177;
  FUN_1920_04df();
  local_102[0] = 0;
  while ((uVar1 = (undefined2)((ulong)param_4 >> 0x10), ((byte *)param_4)[*param_3] != 0x5c &&
         (*param_3 <= (int)(uint)*param_4))) {
    puVar4 = local_304;
    uVar5 = unaff_SS;
    FUN_1920_09da(local_102,unaff_SS);
    puVar2 = local_204;
    uVar3 = unaff_SS;
    FUN_1920_0b0a(CONCAT11((char)((uint)*param_3 >> 8),((byte *)param_4)[*param_3]));
    FUN_1920_0a67(puVar2,uVar3);
    FUN_1920_09f4(0xff,local_102,unaff_SS,puVar4,uVar5);
    *param_3 = *param_3 + 1;
  }
  FUN_1920_09f4(0xff,(int)param_5,(int)((ulong)param_5 >> 0x10),local_102,unaff_SS);
  *param_3 = *param_3 + 1;
  if ((int)(uint)*param_4 < *param_3) {
    *param_2 = 1;
  }
  else {
    *param_2 = 0;
  }
  return;
}


