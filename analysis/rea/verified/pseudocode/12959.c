/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x12959 FUN_1000_2959
 * ev_2cd88f535baf932f39b0b95693fdc6a110c708e96bad574ffa6ff05d52f1fb81
 */

void __cdecl16near FUN_1000_2959(void)

{
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  if (*(char *)0x79b8 == '\x01') {
    FUN_1000_07fa(0x1c,0xf,0xa0,*(int *)0x79ee << 3,0,*(int *)0x79ee * -4 + 0xa0);
    *(int *)0x79f0 = *(int *)0x79ee * -4 + 0x5a4;
    *(undefined2 *)0x79f2 = *(undefined2 *)0x79f0;
  }
  else if (*(char *)0x79b8 == '\x02') {
    FUN_1000_07fa(0x1c,0xf,0xa0,0xa0,0,0);
    FUN_1000_07fa(0xc,4,0xa0,0xa0,0,0xa0);
    *(undefined2 *)0x79f0 = 0x504;
    *(undefined2 *)0x79f2 = 0x5a4;
  }
  return;
}


