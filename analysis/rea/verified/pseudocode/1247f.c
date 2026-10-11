/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1247f FUN_1000_247f
 * ev_cb495d5f5424b3f3896361d7a5e675637d2cdcc61704b97b2c24186d8e3067fc
 */

void FUN_1000_247f(void)

{
  code *pcVar1;
  int iVar2;
  undefined2 uVar3;
  undefined1 uVar4;
  int in_CX;
  undefined2 extraout_DX;
  undefined2 extraout_DX_00;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  FUN_184a_029c(500);
  do {
    do {
      uVar4 = (undefined1)in_CX;
      pcVar1 = (code *)swi(0x21);
      (*pcVar1)();
      *(undefined1 *)0x79a8 = (char)((uint)extraout_DX >> 8);
      *(undefined1 *)0x79a9 = (char)extraout_DX;
      *(undefined1 *)0x79a4 = uVar4;
      FUN_184a_029c(1000);
      pcVar1 = (code *)swi(0x21);
      (*pcVar1)();
      *(undefined1 *)0x79ab = (char)((uint)extraout_DX_00 >> 8);
      *(undefined1 *)0x79ac = (char)extraout_DX_00;
      *(undefined1 *)0x79aa = uVar4;
      in_CX = ((uint)*(byte *)0x79ab - (uint)*(byte *)0x79a8) * 100;
      iVar2 = (((uint)*(byte *)0x79aa - (uint)*(byte *)0x79a4) * 6000 + in_CX +
              (uint)*(byte *)0x79ac) - (uint)*(byte *)0x79a9;
    } while (iVar2 < 1);
  } while (299 < iVar2);
  FUN_1920_0f0f();
  FUN_1920_0f01();
  uVar3 = FUN_1920_0f1b();
  *(undefined2 *)0x78ce = uVar3;
  FUN_1920_0f0f();
  FUN_1920_0f01();
  uVar3 = FUN_1920_0f1b();
  *(undefined2 *)0x78d0 = uVar3;
  FUN_1920_0f0f();
  FUN_1920_0f01();
  uVar3 = FUN_1920_0f1b();
  *(undefined2 *)0x78ca = uVar3;
  if (*(uint *)0x78ca < 2) {
    *(undefined2 *)0x78ca = 2;
  }
  return;
}


