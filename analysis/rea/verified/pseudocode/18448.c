/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x18448 FUN_183f_0058
 * ev_f693ce423c5795036fbefe54f11e398fcd2fc837b96be6d4f39807f2ec8322bb
 */

void __cdecl16near FUN_183f_0058(void)

{
  byte *pbVar1;
  undefined2 in_AX;
  int iVar2;
  int iVar3;
  byte *pbVar4;
  int unaff_DI;
  byte *pbVar5;
  byte *pbVar6;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool in_CF;
  
  if (!in_CF) {
    pbVar5 = (byte *)(unaff_DI + 0x1e);
    iVar2 = 0x100;
    do {
      if (iVar2 == 0) break;
      iVar2 = iVar2 + -1;
      pbVar1 = pbVar5;
      pbVar5 = pbVar5 + 1;
    } while (*pbVar1 != 0);
    iVar3 = CONCAT11((char)((uint)iVar2 >> 8),~(byte)iVar2);
    pbVar6 = pbVar5;
    while( true ) {
      pbVar6 = pbVar6 + -1;
      pbVar4 = pbVar5 + -2;
      if (iVar3 == 0) break;
      iVar3 = iVar3 + -1;
      pbVar5 = pbVar5 + -1;
      *pbVar6 = *pbVar4;
    }
    *pbVar6 = ~(byte)iVar2;
    in_AX = 0;
  }
  *(undefined2 *)0x7f5a = in_AX;
  return;
}


