/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x12e8d FUN_1000_2e8d
 * ev_f31d69ebe2b0a22a96659207006b4d0c2c268a290c1bf997eb3cdfff94121104
 */

void FUN_1000_2e8d(undefined2 param_1,undefined2 *param_2)

{
  undefined2 *puVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  uVar2 = (undefined2)((ulong)param_2 >> 0x10);
  puVar1 = (undefined2 *)param_2;
  *(undefined2 *)0x2082 = 0;
  while( true ) {
    puVar1[*(int *)0x2082 + 2] = *(undefined2 *)0x2070;
    puVar1[*(int *)0x2082 + 0xc] = *(undefined2 *)0x2070;
    if (*(int *)0x2082 == 9) break;
    *(int *)0x2082 = *(int *)0x2082 + 1;
  }
  *(undefined1 *)(puVar1 + 0x16) = 2;
  *param_2 = 0;
  puVar1[1] = 0;
  return;
}


