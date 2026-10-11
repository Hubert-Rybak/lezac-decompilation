/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x13358 FUN_1000_3358
 * ev_c69ae3c3dc71c115b7423325f0996508cd84ea59b3fae329dc9105df16c955e5
 */

void FUN_1000_3358(int param_1)

{
  char *pcVar1;
  byte bVar2;
  byte bVar3;
  int iVar4;
  undefined2 unaff_DS;
  byte *local_c;
  uint local_4;
  
  FUN_1920_04df();
  if (param_1 <= (int)(uint)*(byte *)0x208d) {
    bVar3 = *(byte *)(param_1 * 0x26 + 0x1baf);
    FUN_18ac_0594(bVar3);
    bVar2 = *(byte *)0x208d;
    if (param_1 <= (int)(bVar2 - 1)) {
      for (local_4 = param_1;
          FUN_1920_090e(0x26,local_4 * 0x26 + 0x1bae,unaff_DS,(local_4 + 1) * 0x26 + 0x1bae,unaff_DS
                       ), local_4 != bVar2 - 1; local_4 = local_4 + 1) {
      }
    }
    *(char *)0x208d = *(char *)0x208d + -1;
    bVar2 = *(byte *)0x208d;
    if (bVar2 < 0x8000) {
      local_4 = 0;
      while( true ) {
        if (bVar3 < *(byte *)(local_4 * 0x26 + 0x1baf)) {
          pcVar1 = (char *)(local_4 * 0x26 + 0x1baf);
          *pcVar1 = *pcVar1 + -1;
        }
        if (local_4 == bVar2) break;
        local_4 = local_4 + 1;
      }
    }
    if ((*(char *)0x79f9 != '\0') && (bVar2 = *(byte *)0x79f9, bVar2 != 0)) {
      local_4 = 1;
      while( true ) {
        iVar4 = local_4 * 0x10;
        _local_c = (byte *)CONCAT22(unaff_DS,(byte *)(iVar4 + 0x79ea));
        if (bVar3 < *_local_c) {
          *_local_c = *_local_c - 1;
        }
        if (bVar3 < *(byte *)(iVar4 + 0x79eb)) {
          *(char *)(iVar4 + 0x79eb) = *(char *)(iVar4 + 0x79eb) + -1;
        }
        if (local_4 == bVar2) break;
        local_4 = local_4 + 1;
      }
    }
  }
  return;
}


