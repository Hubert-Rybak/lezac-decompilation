/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x1432a FUN_1000_432a
 * ev_85826038c8eb0b24f92fe8b21f0b791e101e7158fe58b1b9ced69e65b0ee8b27
 */

void __cdecl16near FUN_1000_432a(void)

{
  byte bVar1;
  byte bVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_DS;
  undefined2 local_e;
  undefined2 local_a;
  undefined2 local_4;
  
  FUN_1920_04df();
  bVar1 = *(byte *)0x79f9;
  if (bVar1 != 0) {
    local_4 = 1;
    while( true ) {
      iVar5 = local_4 * 0x10;
      _local_a = (byte *)CONCAT22(unaff_DS,(byte *)(iVar5 + 0x79ea));
      bVar2 = *_local_a;
      _local_e = (int *)CONCAT22(unaff_DS,(int *)((uint)bVar2 * 8 + -0x3de2));
      if (*(char *)(iVar5 + 0x79ed) == -1) {
        *(byte *)(iVar5 + 0x79f0) = *(char *)(iVar5 + 0x79f0) + *(char *)(iVar5 + 0x79ec) & 0x7f;
        iVar4 = (*(byte *)(iVar5 + 0x79f0) + 0x20 & 0x7f) * 6;
        FUN_1920_0f0f(*(undefined2 *)(iVar4 + 0x7bda),*(undefined2 *)(iVar4 + 0x7bdc),
                      *(undefined2 *)(iVar4 + 0x7bde));
        FUN_1920_0efb();
        iVar4 = FUN_1920_0f13();
        *(int *)(iVar5 + 0x79f5) = *_local_e + *(int *)(iVar5 + 0x79f1) + iVar4;
        iVar4 = (uint)*(byte *)(iVar5 + 0x79f0) * 6;
        FUN_1920_0f0f(*(undefined2 *)(iVar4 + 0x7bda),*(undefined2 *)(iVar4 + 0x7bdc),
                      *(undefined2 *)(iVar4 + 0x7bde));
        FUN_1920_0efb();
        iVar4 = FUN_1920_0f13();
        *(int *)(iVar5 + 0x79f7) =
             *(int *)((uint)bVar2 * 8 + -0x3de0) + *(int *)(iVar5 + 0x79f3) + iVar4;
      }
      else {
        uVar3 = FUN_1920_092a();
        *(undefined2 *)(iVar5 + 0x79f5) = uVar3;
        iVar4 = FUN_1920_092a();
        *(int *)(iVar5 + 0x79f7) = *(char *)(iVar5 + 0x79f9) + iVar4;
      }
      if (local_4 == bVar1) break;
      local_4 = local_4 + 1;
    }
  }
  return;
}


