/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x13fa6 FUN_1000_3fa6
 * ev_acb41a3719b37a9e70042766d47d6fdea8d2893feecccd26ef3e7dbe984ec63a
 */

void FUN_1000_3fa6(int param_1,undefined2 param_2,undefined1 param_3,undefined2 param_4,
                  undefined2 param_5)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 *puVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 uVar7;
  undefined1 uVar8;
  undefined2 in_stack_00000016;
  int in_stack_00000018;
  undefined2 in_stack_0000001a;
  undefined4 uVar9;
  undefined2 *puVar10;
  
  uVar9 = 0x10003fb1;
  FUN_1920_04df();
  FUN_1920_0f01(uVar9);
  FUN_1920_0efb();
  FUN_1920_0f01();
  FUN_1920_0efb();
  FUN_1920_0f01();
  FUN_1920_0efb();
  do {
    uVar8 = *(uint *)0x2076 == 0xc6;
    if (*(uint *)0x2076 < 0xc6) {
      *(int *)0x2076 = *(int *)0x2076 + 1;
      *(undefined1 *)(*(int *)0x2076 + 0x78d5) = param_3;
      puVar10 = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(*(int *)0x2076 * 0xb + 0x2093));
      *puVar10 = in_stack_0000001a;
      uVar3 = 0;
      uVar1 = FUN_1920_0f0f();
      uVar6 = in_stack_00000016;
      FUN_1920_1021(0x1920,uVar1,param_5,uVar3);
      FUN_1920_0efb();
      uVar8 = FUN_1920_0f13();
      *(undefined1 *)((undefined2 *)puVar10 + 2) = uVar8;
      uVar3 = 0;
      uVar1 = FUN_1920_0f0f();
      iVar4 = in_stack_00000018;
      FUN_1920_1034(uVar1,uVar6,uVar3);
      FUN_1920_0efb();
      iVar2 = FUN_1920_0f13();
      iVar4 = iVar4 + (uint)(iVar2 != 0);
      uVar7 = iVar4 != 0;
      uVar8 = iVar4 == 0;
      uVar6 = (undefined2)((ulong)puVar10 >> 0x10);
      puVar5 = (undefined2 *)puVar10;
      *(char *)((int)puVar5 + 5) = -(char)iVar2;
      *(undefined1 *)(puVar5 + 3) = 0;
      *(undefined1 *)((int)puVar5 + 7) = 0;
      *(undefined1 *)((int)puVar5 + 9) = 0x75;
      *(undefined1 *)(puVar5 + 4) = *(undefined1 *)(param_1 + -3);
      *(undefined1 *)(puVar5 + 5) = *(undefined1 *)(param_1 + -4);
    }
    else {
      uVar7 = false;
    }
    FUN_1920_0ee9();
    param_5 = in_stack_00000016;
    FUN_1920_0f0b();
  } while ((bool)uVar7 || (bool)uVar8);
  return;
}


