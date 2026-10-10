/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x195d3 FUN_1920_03d3
 * ev_5229c9d588349d66a812adffcbd67b5a5b866a09f9e373ff619d2b14df71758b
 */

void __cdecl16near FUN_1920_03d3(void)

{
  undefined4 *puVar1;
  int in_AX;
  undefined4 *in_CX;
  uint in_BX;
  undefined4 *puVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint unaff_DS;
  undefined4 uVar5;
  
  if ((((in_AX != 0) && (((uint)in_CX & 0xfff7) == 0)) &&
      ((*(uint *)0x1ad8 < in_BX ||
       ((*(uint *)0x1ad8 <= in_BX && ((undefined4 *)*(uint *)0x1ad6 <= in_CX)))))) &&
     ((in_BX < *(uint *)0x1adc ||
      ((in_BX <= *(uint *)0x1adc && (in_CX < (undefined4 *)*(uint *)0x1ada)))))) {
    uVar5 = FUN_1920_0491();
    *(int *)(in_CX + 1) = (int)uVar5;
    *(undefined2 *)((int)in_CX + 6) = (int)((ulong)uVar5 >> 0x10);
    puVar3 = (undefined4 *)0x1ae2;
    do {
      do {
        uVar4 = unaff_DS;
        puVar2 = puVar3;
        puVar1 = (undefined4 *)*puVar2;
        unaff_DS = (uint)((ulong)puVar1 >> 0x10);
        puVar3 = (undefined4 *)puVar1;
      } while (unaff_DS < in_BX);
      if (in_BX < unaff_DS) goto LAB_1920_0424;
    } while (puVar3 < in_CX);
    if (in_CX != puVar3) {
LAB_1920_0424:
      FUN_1920_0432(puVar2,uVar4);
      FUN_1920_0432();
      return;
    }
  }
  return;
}


