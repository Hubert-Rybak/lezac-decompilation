/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x14ee7 FUN_1000_4ee7
 * ev_e0bf23cf40b03e844ae7993586f5598bf5b08ed7a0dd2e24ea31483a28694a9e
 */

void __cdecl16near FUN_1000_4ee7(void)

{
  undefined1 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int in_BX;
  int *piVar5;
  undefined1 *puVar6;
  int *piVar7;
  undefined2 uVar8;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x208c = 0;
  *(undefined2 *)0x2078 = 0;
  FUN_1000_4e48();
  if (*(char *)0x661e != '\x01') {
    uVar8 = (undefined2)((ulong)*(undefined4 *)0x6612 >> 0x10);
    *(undefined1 *)0x208c = 1;
    iVar4 = *(int *)0x2092;
    if (-*(int *)0xc204 == iVar4) {
      *(undefined2 *)0x2060 = *(undefined2 *)0x2068;
      *(undefined2 *)0x2062 = 2;
      *(undefined2 *)0x2064 = *(undefined2 *)0x2066;
    }
    else if (iVar4 == *(int *)0xc204) {
      *(undefined2 *)0x2060 = *(undefined2 *)0x206c;
      *(undefined2 *)0x2062 = 0xfffe;
      *(int *)0x2064 = -*(int *)0x2066;
    }
    else if (iVar4 == 1) {
      *(undefined2 *)0x2060 = *(undefined2 *)0x206a;
      *(undefined2 *)0x2062 = 0xfffe;
      *(undefined2 *)0x2064 = *(undefined2 *)0x2066;
    }
    else {
      *(undefined2 *)0x2060 = *(undefined2 *)0x2068;
      *(undefined2 *)0x2062 = 2;
      *(undefined2 *)0x2064 = *(undefined2 *)0x2066;
    }
    piVar5 = (int *)(*(int *)0x2060 << 1);
    iVar4 = *(int *)0x205c;
    do {
      iVar3 = *(int *)0x205a;
      piVar7 = piVar5;
      do {
        if (*piVar7 == *(int *)0x205e) {
          *(int *)(in_BX + (int)piVar7) = *piVar7;
          *piVar7 = 0;
          puVar6 = (undefined1 *)((uint)piVar7 >> 1);
          iVar2 = *(int *)0x2092;
          uVar8 = *(undefined2 *)0xc1fe;
          LOCK();
          uVar1 = *puVar6;
          *puVar6 = 0;
          UNLOCK();
          puVar6[iVar2] = uVar1;
          in_BX = *(int *)0x2090;
          piVar7 = (int *)((int)puVar6 << 1);
          uVar8 = *(undefined2 *)0x206e;
        }
        piVar7 = (int *)((int)piVar7 + *(int *)0x2062);
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      piVar5 = (int *)((int)piVar5 + *(int *)0x2064);
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    *(int *)0x2068 = *(int *)0x2068 + *(int *)0x2092;
    *(int *)0x206a = *(int *)0x206a + *(int *)0x2092;
    *(int *)0x206c = *(int *)0x206c + *(int *)0x2092;
  }
  return;
}


