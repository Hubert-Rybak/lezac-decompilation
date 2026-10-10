/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x193bf FUN_1920_01bf
 * ev_c6bef1de2beab0d76fb93ffd7b7760b5d4dc6adaf6352225d983a499190c3420
 */

undefined2 __cdecl16near FUN_1920_01bf(void)

{
  uint in_AX;
  undefined2 uVar1;
  undefined1 uVar2;
  byte in_CL;
  
  uVar1 = CONCAT11((char)((in_AX & 0xff) % (uint)in_CL),(char)((in_AX & 0xff) / (uint)in_CL) + '0');
  FUN_1920_01e7(uVar1);
  uVar2 = (undefined1)((uint)uVar1 >> 8);
  return CONCAT11(uVar2,uVar2);
}


