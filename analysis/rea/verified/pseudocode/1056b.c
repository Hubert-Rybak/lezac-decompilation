/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1056b FUN_1000_056b
 * ev_bbc475e4773072583674a6252df69a659a1df18aaebad3ec955e1eb152beafa8
 */

void FUN_1000_056b(byte param_1,undefined4 param_2)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  undefined2 local_e;
  undefined2 local_a;
  undefined2 local_4;
  
  FUN_1920_04df();
  local_4 = 1;
  bVar1 = false;
  uVar5 = (undefined2)((ulong)param_2 >> 0x10);
  iVar2 = (int)param_2;
  *(undefined1 *)(iVar2 + 2) = 0;
  while ((local_4 <= (int)(uint)*(byte *)0x79a5 && (!bVar1))) {
    iVar3 = local_4 * 7;
    if ((*(byte *)(iVar3 + 0x771d) & param_1) != 0) {
      iVar4 = (uint)*(byte *)(iVar2 + 1) * 8;
      _local_e = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar4 + -0x3de2));
      *_local_e = *(undefined2 *)(iVar3 + 0x7719);
      *(undefined2 *)(iVar4 + -0x3de0) = *(undefined2 *)(iVar3 + 0x771b);
      bVar1 = true;
    }
    local_4 = local_4 + 1;
  }
  if (!bVar1) {
    iVar2 = (uint)*(byte *)(iVar2 + 1) * 8;
    _local_a = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar2 + -0x3de2));
    *_local_a = 0x11;
    *(undefined2 *)(iVar2 + -0x3de0) = 0x11;
  }
  return;
}


