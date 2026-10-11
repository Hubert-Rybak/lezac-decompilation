/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x11287 FUN_1000_1287
 * ev_3cf5e3a49bf1f99b1f4e801eff830541ef13d4515bb6b41f831eed1e04a1774a
 */

void FUN_1000_1287(int param_1)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  char cVar5;
  int iVar6;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  iVar6 = param_1 + 0x10;
  cVar5 = '\0';
  uVar4 = 9;
  do {
    iVar1 = *(int *)(iVar6 + 0x785e);
    if (iVar1 != *(int *)(iVar6 + 0x7872)) {
      uVar2 = iVar1 + 8;
      if ((uVar4 & 1) != 0) {
        uVar2 = iVar1 + 0x10;
      }
      cVar5 = cVar5 + '\x01';
      uVar3 = uVar2 - 0x280;
      if (uVar2 - 0x280 < *(uint *)0x2070) {
        uVar3 = uVar2;
      }
      *(uint *)(iVar6 + 0x785e) = uVar3;
    }
    iVar6 = iVar6 + -2;
    uVar4 = uVar4 - 1;
  } while (uVar4 != 0);
  if (cVar5 == '\0') {
    *(undefined1 *)(param_1 + 0x7886) = 2;
  }
  return;
}


