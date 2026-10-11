/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1079d FUN_1000_079d
 * ev_82cc2181a33288f8c17b2ce6b87c4bed0c2b35445b84c5d5deb626e6215f1168
 */

void __cdecl16near FUN_1000_079d(void)

{
  char cVar1;
  int iVar2;
  char cVar3;
  undefined2 unaff_DS;
  
  iVar2 = 6;
  cVar3 = -0x1a;
  cVar1 = *(char *)0x79ad;
  do {
    out(0x3c8,cVar3);
    out(0x3c9,cVar1);
    out(0x3c9,0);
    out(0x3c9,0);
    cVar1 = cVar1 + '\a';
    if ('?' < cVar1) {
      cVar1 = '\x14';
    }
    cVar3 = cVar3 + '\x01';
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}


