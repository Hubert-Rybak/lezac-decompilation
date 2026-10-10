/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19774 FUN_1920_0574
 * ev_d834993e05061ebbf2a045f72260ba456325f3c5739252d9448894a436b3aee3
 */

void __stdcall16far FUN_1920_0574(undefined4 param_1)

{
  int iVar1;
  undefined2 in_DX;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  bool bVar4;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  iVar1 = *(int *)(iVar2 + 2);
  if ((iVar1 == -0x284f) || (iVar1 == -0x284e)) {
    FUN_1920_05c1(iVar2,uVar3);
  }
  else if (iVar1 != -0x2850) {
    *(undefined2 *)0x1afc = 0x66;
    return;
  }
  bVar4 = true;
  *(undefined2 *)(iVar2 + 2) = in_DX;
  *(undefined2 *)(iVar2 + 8) = 0;
  *(undefined2 *)(iVar2 + 10) = 0;
  FUN_1920_05fd();
  if (!bVar4) {
    *(undefined2 *)(iVar2 + 2) = 0xd7b0;
  }
  return;
}


