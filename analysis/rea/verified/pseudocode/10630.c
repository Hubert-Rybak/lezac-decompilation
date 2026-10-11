/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x10630 FUN_1000_0630
 * ev_5c433dd1a7dd2a1b12e3ff922e849c0d3cc41ebc01dbaba63deb0071e5d82e6f
 */

void __cdecl16near FUN_1000_0630(void)

{
  int iVar1;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  int local_84;
  undefined1 local_82 [124];
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  uStack_4 = 0x1000;
  uStack_6 = 0x63b;
  FUN_1920_04df();
  FUN_1920_15ca(0x625,0x1920,local_82,unaff_SS);
  FUN_1920_15f8(1,local_82,unaff_SS);
  FUN_1920_16e3(0,0,2,&local_84,unaff_SS,local_82,unaff_SS);
  FUN_1920_16e3(0,0,local_84 * 6,(int)*(undefined4 *)0x79c0,
                (int)((ulong)*(undefined4 *)0x79c0 >> 0x10),local_82,unaff_SS);
  FUN_1920_1679(local_82,unaff_SS);
  iVar1 = FUN_1920_04a2();
  if (iVar1 != 0) {
    FUN_1000_00a3(1);
  }
  return;
}


