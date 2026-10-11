/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1a1c2 FUN_1920_0fc2
 * ev_c23d1471cebae0066af48911592d943c4cc7f817a145b2da981016007b8a524e
 */

uint __cdecl16far FUN_1920_0fc2(void)

{
  char cVar1;
  byte bVar2;
  uint in_AX;
  uint uVar3;
  uint in_DX;
  undefined2 in_BX;
  undefined2 uVar4;
  undefined4 uVar5;
  
  cVar1 = (char)in_AX;
  if (cVar1 != '\0') {
    if ((in_DX & 0x8000) != 0) {
      uVar3 = FUN_1920_00e2();
      return uVar3;
    }
    do {
      uVar4 = in_BX;
      FUN_1920_0f53();
      uVar5 = FUN_1920_0f35();
      in_AX = (uint)(byte)((char)uVar5 - 1);
      bVar2 = FUN_1920_0be1(in_AX,uVar4,(int)((ulong)uVar5 >> 0x10));
    } while ((byte)(((char)(cVar1 + -0x80) >> 1) + 0x6cU) <= bVar2);
  }
  return in_AX;
}


