/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x102d1 FUN_1000_02d1
 * ev_fb2c2ab47e25857cf37861cf598e7ab1df106df6bf9e247afe916d609a86fdef
 */

void __cdecl16near FUN_1000_02d1(void)

{
  int iVar1;
  int in_AX;
  int unaff_BP;
  undefined2 unaff_SS;
  
  do {
    *(int *)(unaff_BP + -2) = *(int *)(unaff_BP + -2) + in_AX;
    if (100 < *(int *)(unaff_BP + -2)) {
      *(int *)(unaff_BP + -2) = *(int *)(unaff_BP + -2) + -100;
      *(int *)(unaff_BP + -0xe) = *(int *)(unaff_BP + -0xe) + 1;
    }
    do {
      *(char *)*(undefined2 *)(unaff_BP + -0xc) =
           (char)((ulong)*(uint *)(unaff_BP + -0xe) % 7) + -0x50;
      *(int *)(unaff_BP + -0xc) = *(int *)(unaff_BP + -0xc) + 1;
      if (*(int *)(unaff_BP + -10) == -0x601) {
        return;
      }
      *(int *)(unaff_BP + -10) = *(int *)(unaff_BP + -10) + 1;
      *(int *)(unaff_BP + -2) = *(int *)(unaff_BP + -2) + *(int *)(unaff_BP + -6);
      if (100 < *(int *)(unaff_BP + -2)) {
        *(int *)(unaff_BP + -2) = *(int *)(unaff_BP + -2) + -100;
        *(int *)(unaff_BP + -0xe) = *(int *)(unaff_BP + -0xe) + 1;
      }
    } while (*(uint *)(unaff_BP + -10) % 0x140 != 0);
    FUN_1920_0f0f();
    FUN_1920_0efb();
    FUN_1920_0f13();
    FUN_1920_0945();
    FUN_1920_0efb();
    iVar1 = FUN_1920_0f13();
    in_AX = *(int *)(unaff_BP + -8) + iVar1;
  } while( true );
}


