/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x13a7e FUN_1000_3a7e
 * ev_a4d4cfe25086311b6a0c3083f06f27761f7596ce9f7c2f823cb521044642ea89
 */

void __cdecl16near FUN_1000_3a7e(void)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  undefined2 unaff_DS;
  
  uVar1 = *(uint *)0x2074;
  if ((uVar1 & 0x8000) == 0) {
    *(undefined1 *)0x661e = 0;
  }
  else if ((uVar1 & 0x7fff) < 0x4000) {
    if (*(int *)0x2080 + -1 != -1) {
      uVar2 = (*(int *)0x2080 + -1) * 0xf + 4;
      iVar3 = *(int *)0x2080;
      do {
        if (*(uint *)(uVar2 + 0x6620) == uVar1) {
          *(undefined1 *)0x661e = *(undefined1 *)(uVar2 + 0x6622);
          *(int *)0x2074 = uVar2 / 0xf + 1;
          return;
        }
        uVar2 = uVar2 - 0xf;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
    }
  }
  else if (*(int *)0x207e + -1 != 0xc6) {
    uVar2 = (*(int *)0x207e + -1) * 0xb + 2;
    iVar3 = *(int *)0x207e + -199;
    do {
      if (*(uint *)(uVar2 + 0x209e) == uVar1) {
        *(undefined1 *)0x661e = *(undefined1 *)(uVar2 + 0x20a0);
        *(int *)0x2074 = uVar2 / 0xb + 1;
        return;
      }
      uVar2 = uVar2 - 0xb;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  return;
}


