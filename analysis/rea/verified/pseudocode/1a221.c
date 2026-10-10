/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a221 FUN_1920_1021
 * ev_81513a94f2edbbdc4e04b2f0cdc6981d4ce99a7d0f9cc77caa8b1e37136c282a
 */

void FUN_1920_1021(void)

{
  byte bVar1;
  undefined2 uVar2;
  uint extraout_DX;
  uint uVar3;
  uint extraout_DX_00;
  bool bVar4;
  undefined1 uVar5;
  
  bVar1 = FUN_1920_0be5();
  uVar3 = extraout_DX;
  if (bVar1 != 0) {
    uVar3 = extraout_DX ^ 0x8000;
  }
  if (0x6b < bVar1) {
    uVar2 = 0x2183;
    bVar4 = false;
    FUN_1920_0e24();
    if (!bVar4) {
      FUN_1920_0f53();
      FUN_1920_0fae(uVar2,0xdaa2,0x490f);
      FUN_1920_0f49();
      uVar3 = extraout_DX_00;
    }
    uVar5 = 0;
    if ((uVar3 & 0x8000) != 0) {
      FUN_1920_0f35();
    }
    FUN_1920_0e24();
    if (!(bool)uVar5) {
      FUN_1920_0f3f();
    }
    bVar1 = FUN_1920_0e24();
    if (!(bool)uVar5) {
      bVar1 = FUN_1920_0be5();
    }
    if (0x6b < bVar1) {
      FUN_1920_1340();
    }
  }
  return;
}


