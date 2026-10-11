/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19200 FUN_1920_0000
 * ev_1f3912c207000738d92b0bb460a225f378d91be0825b35b0b2e998f45f664eab
 */

void __cdecl16far FUN_1920_0000(void)

{
  code *pcVar1;
  int iVar2;
  undefined2 in_BX;
  undefined2 *puVar3;
  undefined2 unaff_ES;
  int unaff_SS;
  
  DAT_1aa2_1aca = ((uint)&stack0x0013 >> 4) + unaff_SS;
  DAT_1aa2_1ace = DAT_1aa2_1aca + DAT_1aa2_1ac4;
  DAT_1aa2_1ae0 = *(undefined2 *)0x2;
  DAT_1aa2_1aea = 0xa9;
  DAT_1aa2_1aec = 0x1920;
  puVar3 = (undefined2 *)0xc69e;
  iVar2 = 0x13;
  DAT_1aa2_1acc = DAT_1aa2_1aca;
  DAT_1aa2_1ad8 = DAT_1aa2_1ace;
  DAT_1aa2_1adc = DAT_1aa2_1ace;
  DAT_1aa2_1ae4 = DAT_1aa2_1ace;
  DAT_1aa2_1af8 = unaff_ES;
  do {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    *puVar3 = in_BX;
    puVar3[1] = unaff_ES;
    puVar3 = puVar3 + 2;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  FUN_1920_04f7();
  FUN_1920_0567();
  FUN_1920_04f7();
  FUN_1920_056c();
  return;
}


