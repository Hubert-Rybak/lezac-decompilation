/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x18585 FUN_184a_00e5
 * ev_97032554a184468666ea5e887d31f7660eb067d75b9ef6414ae874286cad7c19
 */

void __cdecl16near FUN_184a_00e5(void)

{
  uint uVar1;
  uint uVar2;
  undefined1 uVar3;
  byte extraout_DL;
  byte bVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  
  uVar1 = FUN_184a_060b();
  FUN_184a_060b();
  uVar3 = 0;
  bVar4 = extraout_DL;
  if ((extraout_DL == 0) && (bVar4 = 0x18, (byte)uVar1 < 4)) {
    uVar3 = 1;
  }
  uVar5 = CONCAT11(bVar4,(char)(uVar1 >> 8) + -1);
  uVar2 = uVar1 & 0xff;
  if (0x18 < bVar4) {
    uVar2 = CONCAT11(1,(byte)uVar1);
  }
  *(uint *)0x7f60 = uVar2;
  *(undefined2 *)0x7f6a = uVar5;
  *(undefined1 *)0x7f5f = uVar3;
  *(undefined1 *)0x7f5e = 1;
  *(undefined2 *)0x7f64 = 0;
  *(undefined2 *)0x7f66 = uVar5;
  return;
}


