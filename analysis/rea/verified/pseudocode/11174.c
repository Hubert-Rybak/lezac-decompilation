/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x11174 FUN_1000_1174
 * ev_0bd88da73d20a4751ee4e3d8a1083d4f7800b40223ac2027106c1c122a4476d8
 */

void FUN_1000_1174(int param_1)

{
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  *(int *)0x2072 = param_1 + 0x501;
  *(int *)0x2074 = (uint)*(byte *)0x79b4 * 0x40 + *(int *)0xc208;
  FUN_1000_1156();
  *(int *)0x2072 = *(int *)0x2072 + 0x1400;
  *(int *)0x2074 = *(int *)0xc208 + 0x1d40;
  FUN_1000_1156();
  return;
}


