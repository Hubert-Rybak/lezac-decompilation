/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x11239 FUN_1000_1239
 * ev_61e82db3d8647bc855ade43a5440e4b167bec206a43d1ceeadfa9b1c780d4acf
 */

void FUN_1000_1239(int param_1)

{
  bool bVar1;
  int iVar2;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  *(undefined2 *)0x2072 = *(undefined2 *)0x2074;
  param_1 = param_1 + 0x10;
  iVar2 = 9;
  bVar1 = false;
  do {
    if ((bVar1) || (*(int *)(param_1 + 0x785e) != *(int *)0x2070)) {
      *(int *)0x2074 = *(int *)(param_1 + 0x785e);
      FUN_1000_1156();
      bVar1 = true;
    }
    *(int *)0x2072 = *(int *)0x2072 + 9;
    param_1 = param_1 + -2;
    if (iVar2 == 2) {
      bVar1 = true;
    }
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}


