/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x18fd7 FUN_18ac_0517
 * ev_cd6dc5f9d8312b6686c7b0b00eced426dd9b1b06beedbe7b8ac02da6da94419e
 */

void __stdcall16far FUN_18ac_0517(undefined2 param_1,undefined2 param_2,int param_3)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined2 local_6;
  
  FUN_1920_04df();
  if (*(byte *)0xc496 < 0x21) {
    iVar1 = (uint)*(byte *)0xc496 * 8;
    _local_6 = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar1 + -0x3de2));
    *_local_6 = param_2;
    *(undefined2 *)(iVar1 + -0x3de0) = param_1;
    *(undefined2 *)(iVar1 + -0x3ddc) = *(undefined2 *)(param_3 * 4 + -0x3cdc);
    *(undefined1 *)(iVar1 + -0x3dde) = *(undefined1 *)(param_3 * 4 + -0x3cde);
    *(undefined1 *)(iVar1 + -0x3ddd) = *(undefined1 *)(param_3 * 4 + -0x3cdd);
    *(char *)0xc496 = *(char *)0xc496 + '\x01';
  }
  return;
}


