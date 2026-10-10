/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a234 FUN_1920_1034
 * ev_82f028c0e42b539d27e639e6009c081270187789962812ca49835ff8817de371
 */

void __cdecl16far FUN_1920_1034(void)

{
  byte in_AL;
  byte bVar1;
  undefined2 uVar2;
  uint in_DX;
  uint extraout_DX;
  bool bVar3;
  undefined1 uVar4;
  
  if (0x6b < in_AL) {
    uVar2 = 0x2183;
    bVar3 = false;
    FUN_1920_0e24();
    if (!bVar3) {
      FUN_1920_0f53();
      FUN_1920_0fae(uVar2,0xdaa2,0x490f);
      FUN_1920_0f49();
      in_DX = extraout_DX;
    }
    uVar4 = 0;
    if ((in_DX & 0x8000) != 0) {
      FUN_1920_0f35();
    }
    FUN_1920_0e24();
    if (!(bool)uVar4) {
      FUN_1920_0f3f();
    }
    bVar1 = FUN_1920_0e24();
    if (!(bool)uVar4) {
      bVar1 = FUN_1920_0be5();
    }
    if (0x6b < bVar1) {
      FUN_1920_1340();
    }
  }
  return;
}


