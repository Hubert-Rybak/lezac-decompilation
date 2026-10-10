/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x13587 FUN_1000_3587
 * ev_f7dd180b40275254471b27789ef2b0798d2de402a1818fc14d6655f74adc4184
 */

void FUN_1000_3587(int param_1,int param_2)

{
  int iVar1;
  undefined2 unaff_DS;
  
  FUN_1920_04df();
  *(uint *)0xc216 = param_2 - *(int *)0x78bc & 0xfff8;
  *(uint *)0xc218 = param_1 - 0x50U & 0xfff8;
  iVar1 = FUN_1920_092a();
  *(int *)0xc1f0 = (param_2 / 8 - *(int *)0x78be) + iVar1;
  *(int *)0xc20a = param_2 % 8;
  *(int *)0xc20c = param_1 % 8;
  if (*(int *)0xc216 < 0) {
    *(int *)0xc1f0 = *(int *)0xc1f0 - *(int *)0xc216 / 8;
    *(undefined2 *)0xc20a = 0;
    *(undefined2 *)0xc216 = 0;
  }
  else if (*(int *)0x2094 < *(int *)0xc216) {
    *(int *)0xc1f0 = *(int *)0xc1f0 + (*(int *)0x2094 - *(int *)0xc216) / 8;
    *(undefined2 *)0xc20a = 7;
    *(undefined2 *)0xc216 = *(undefined2 *)0x2094;
  }
  if (*(int *)0xc218 < 0) {
    iVar1 = FUN_1920_092a();
    *(int *)0xc1f0 = *(int *)0xc1f0 - iVar1;
    *(undefined2 *)0xc20c = 0;
    *(undefined2 *)0xc218 = 0;
  }
  else if (*(int *)0x2096 < *(int *)0xc218) {
    iVar1 = FUN_1920_092a();
    *(int *)0xc1f0 = *(int *)0xc1f0 - iVar1;
    *(undefined2 *)0xc20c = 7;
    *(undefined2 *)0xc218 = *(undefined2 *)0x2096;
  }
  *(undefined2 *)0xc1f0 = *(undefined2 *)0xc1f0;
  *(int *)0xc20a = *(int *)0xc20a + *(int *)0x2098;
  return;
}


