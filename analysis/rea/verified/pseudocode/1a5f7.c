/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a5f7 FUN_1920_13f7
 * ev_f4f782e8eb80c2adab3371238ca82ab9f5dadca3e566aa7048726d34638ab4a1
 */

void __cdecl16near FUN_1920_13f7(void)

{
  int iVar1;
  long lVar2;
  uint uVar3;
  char cVar4;
  int iVar5;
  undefined2 unaff_DS;
  
  iVar1 = *(int *)0x1b00;
  lVar2 = (ulong)*(uint *)0x1afe * 0x8b50;
  uVar3 = (uint)lVar2;
  iVar5 = *(uint *)0x1afe << 3;
  cVar4 = (char)iVar5;
  iVar5 = (int)((ulong)lVar2 >> 0x10) + CONCAT11((char)((uint)iVar5 >> 8) + cVar4,cVar4) + iVar1 * 5
  ;
  *(int *)0x1afe = uVar3 + 1;
  *(int *)0x1b00 =
       CONCAT11((char)((uint)iVar5 >> 8) + (char)iVar1 * '\x04' + (char)(iVar1 << 7),(char)iVar5) +
       (uint)(0xfffe < uVar3);
  return;
}


