/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1346b FUN_1000_346b
 * ev_a737166f35b5da7ac636a707f73bc0e3528439b3f0f3d9c26b7d6ff416b60c1a
 */

void FUN_1000_346b(undefined2 *param_1,undefined2 *param_2,undefined2 param_3,undefined2 param_4,
                  int *param_5,int *param_6)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  undefined2 in_BX;
  bool bVar6;
  
  FUN_1920_04df();
  iVar4 = *param_6 >> 0xf;
  uVar1 = FUN_1920_0f0f();
  iVar5 = *param_5 >> 0xf;
  uVar3 = in_BX;
  uVar2 = FUN_1920_0f0f();
  bVar6 = true;
  FUN_1920_0f0b(uVar2,uVar3,iVar5,uVar1);
  if (bVar6) {
    bVar6 = true;
    FUN_1920_0f0b(uVar2,uVar3,iVar5,uVar1);
    if (bVar6) goto LAB_1000_353f;
  }
  uVar1 = FUN_1920_0efb();
  FUN_1920_0efb(uVar1,uVar3,iVar5);
  FUN_1920_0ee9();
  uVar3 = FUN_1920_0fc2();
  FUN_1920_0f0f(uVar3,in_BX,iVar4);
  FUN_1920_0f01();
LAB_1000_353f:
  FUN_1920_0efb();
  uVar3 = FUN_1920_0f13();
  *param_2 = uVar3;
  FUN_1920_0efb();
  uVar3 = FUN_1920_0f13();
  *param_1 = uVar3;
  return;
}


