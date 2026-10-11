/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1891e FUN_184a_047e
 * ev_720f4c4bba807e10c3133f8d082411ec2f0c1689cf4f319f8d816d4f9f99680c
 */

void __cdecl16near FUN_184a_047e(void)

{
  char in_AL;
  char extraout_DL;
  undefined2 unaff_DS;
  
  thunk_FUN_184a_060b();
  if (in_AL == '\a') {
    FUN_184a_060b();
  }
  else if (((in_AL != '\b') && (in_AL != '\r')) &&
          ((in_AL == '\n' || (FUN_184a_060b(), *(byte *)0x7f66 < (byte)(extraout_DL + 1U))))) {
    FUN_184a_04d7();
  }
  thunk_FUN_184a_060b();
  return;
}


