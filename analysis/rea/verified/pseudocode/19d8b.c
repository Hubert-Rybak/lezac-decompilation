/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19d8b FUN_1920_0b8b
 * ev_2ed118b47e8bc98d2d64967e6abdbc85d0556d12f541b1a28526c0b5d9df7b92
 */

void __stdcall16far FUN_1920_0b8b(int param_1,int param_2,undefined4 param_3)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_SS;
  undefined1 *puVar3;
  undefined1 *puVar4;
  undefined2 uVar5;
  undefined1 local_202 [256];
  undefined1 local_102 [256];
  
  if (0 < param_1) {
    puVar4 = local_102;
    uVar2 = (undefined2)((ulong)param_3 >> 0x10);
    uVar1 = (undefined2)param_3;
    uVar5 = unaff_SS;
    FUN_1920_0a26(param_2 + -1,1,uVar1,uVar2);
    puVar3 = local_202;
    FUN_1920_0a26(0xff,param_2 + param_1,uVar1,uVar2);
    FUN_1920_0a67(puVar3,unaff_SS);
    FUN_1920_09f4(0xff,uVar1,uVar2,puVar4,uVar5);
  }
  return;
}


