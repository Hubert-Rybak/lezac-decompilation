/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a74e FUN_1920_154e
 * ev_0ac2659d98231f2791d893a1241543756ecc965a7d3ae970411d18e4c2bcf607
 */

void __stdcall16far FUN_1920_154e(int param_1,undefined1 *param_2,int param_3)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int in_CX;
  undefined1 *puVar3;
  undefined1 *puVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  undefined1 local_22 [32];
  
  puVar3 = local_22;
  FUN_1920_143c();
  uVar5 = (undefined2)((ulong)param_2 >> 0x10);
  if (param_1 < param_3) {
    param_3 = param_1;
  }
  if (param_1 < in_CX) {
    in_CX = param_1;
  }
  if (param_3 < in_CX) {
    param_3 = in_CX;
  }
  puVar4 = (undefined1 *)param_2 + 1;
  *param_2 = (char)param_3;
  param_3 = param_3 - in_CX;
  if (param_3 != 0) {
    for (; param_3 != 0; param_3 = param_3 + -1) {
      puVar1 = puVar4;
      puVar4 = puVar4 + 1;
      *puVar1 = 0x20;
    }
  }
  for (; in_CX != 0; in_CX = in_CX + -1) {
    puVar2 = puVar4;
    puVar4 = puVar4 + 1;
    puVar1 = puVar3;
    puVar3 = puVar3 + 1;
    *puVar2 = *puVar1;
  }
  return;
}


