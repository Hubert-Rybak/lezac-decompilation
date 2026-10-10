/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x13184 FUN_1000_3184
 * ev_c284d32fa903319f0f31efd86fc2844abf497be9f6d28247529350e535369d18
 */

void FUN_1000_3184(int param_1)

{
  undefined2 unaff_DS;
  undefined2 local_4;
  
  FUN_1920_04df();
  *(int *)0x2072 = param_1 + 0x519;
  if (*(int *)0x208a != *(int *)0x2088) {
    *(undefined2 *)0x208a = *(undefined2 *)0x2088;
    FUN_1000_0495(0x1aae,unaff_DS,0x1ab2,unaff_DS,0xfff5);
    local_4 = *(int *)0x2086 - *(int *)0x2088;
    if (local_4 < 1) {
      local_4 = 0;
      *(undefined1 *)0x79c5 = 1;
    }
    FUN_1000_313d(2,local_4);
  }
  if (*(char *)0x79b6 != *(char *)0x79b5) {
    FUN_1000_0495(0x1aae,unaff_DS,0x1ab2,unaff_DS,0xfff6);
    *(undefined1 *)0x79b6 = *(undefined1 *)0x79b5;
    *(int *)0x2072 = param_1 + 0x1919;
    if ((uint)*(byte *)0x79b5 < (uint)*(byte *)0x79b3) {
      local_4 = (uint)*(byte *)0x79b3 - (uint)*(byte *)0x79b5;
    }
    else {
      local_4 = 0;
      *(undefined1 *)0x79c6 = 1;
    }
    FUN_1000_313d(2,local_4);
  }
  if ((*(char *)0x79c5 != '\0') && (*(char *)0x79c6 != '\0')) {
    FUN_1000_0495(0x1aae,unaff_DS,0x1ab2,unaff_DS,0xffe0);
  }
  return;
}


