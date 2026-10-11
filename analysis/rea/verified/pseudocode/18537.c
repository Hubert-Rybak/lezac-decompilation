/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x18537 FUN_184a_0097
 * ev_9faa6e98e280be8317e618885eceb1aef79b5b05bcdab952bdbcf8a91109abc1
 */

void __cdecl16near FUN_184a_0097(void)

{
  undefined2 in_AX;
  char extraout_DL;
  
  DAT_0000_0487 = DAT_0000_0487 & 0xfe;
  if (((byte)in_AX != 7) && (3 < (byte)in_AX)) {
    in_AX = CONCAT11((char)((uint)in_AX >> 8),3);
  }
  FUN_184a_060b();
  if ((char)((uint)in_AX >> 8) != '\0') {
    FUN_184a_060b();
    FUN_184a_060b();
    if (extraout_DL == '*') {
      DAT_0000_0487 = DAT_0000_0487 | 1;
      FUN_184a_060b();
      FUN_184a_060b();
    }
  }
  return;
}


