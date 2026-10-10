/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x18834 FUN_184a_0394
 * ev_376253dcbb56051ee3c628be9f7e5295db89066a8b1a43f28ebe8ab4c89c88ab
 */

void __stdcall16far FUN_184a_0394(undefined4 param_1)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  uint extraout_DX;
  uint extraout_DX_00;
  uint extraout_DX_01;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  undefined2 unaff_DS;
  
  uVar8 = (undefined2)((ulong)param_1 >> 0x10);
  iVar6 = (int)param_1;
  uVar3 = *(int *)(iVar6 + 4) - 2;
  uVar9 = (undefined2)((ulong)*(undefined4 *)(iVar6 + 0xc) >> 0x10);
  iVar7 = (int)*(undefined4 *)(iVar6 + 0xc);
  uVar4 = 0;
  uVar5 = *(uint *)(iVar6 + 8);
LAB_184a_03aa:
  do {
    *(undefined1 *)0x7f6d = 0;
    bVar1 = FUN_184a_030f();
    iVar2 = 1;
    if ((bVar1 == 8) || (bVar1 == 0x13)) {
LAB_184a_03ee:
      do {
        if (uVar4 == 0) break;
        FUN_184a_047e();
        FUN_184a_047e();
        FUN_184a_047e();
        uVar4 = uVar4 - 1;
        iVar2 = iVar2 + -1;
        uVar3 = extraout_DX_00;
      } while (iVar2 != 0);
      goto LAB_184a_03aa;
    }
    if (bVar1 == 4) {
LAB_184a_0406:
      do {
        if ((uVar4 == uVar5) || (*(byte *)(uVar4 + iVar7) < 0x20)) break;
        FUN_184a_047e();
        uVar4 = uVar4 + 1;
        iVar2 = iVar2 + -1;
        uVar3 = extraout_DX_01;
      } while (iVar2 != 0);
    }
    else {
      iVar2 = 0;
      if ((bVar1 == 0x1b) || (bVar1 == 1)) goto LAB_184a_03ee;
      if (bVar1 == 6) goto LAB_184a_0406;
      if (bVar1 == 0x1a) {
        if (*(char *)0x7f5d != '\0') {
          *(undefined1 *)(uVar4 + iVar7) = 0x1a;
          iVar7 = uVar4 + 1;
LAB_184a_0430:
          *(undefined2 *)(iVar6 + 8) = 0;
          *(int *)(iVar6 + 10) = iVar7;
          return;
        }
      }
      else {
        if (bVar1 == 0xd) {
          FUN_184a_0477();
          *(undefined2 *)(uVar4 + iVar7) = 0xa0d;
          iVar7 = uVar4 + 2;
          goto LAB_184a_0430;
        }
        if ((0x1f < bVar1) && (uVar4 != uVar3)) {
          *(byte *)(uVar4 + iVar7) = bVar1;
          uVar4 = uVar4 + 1;
          FUN_184a_047e();
          uVar3 = extraout_DX;
          if (uVar5 < uVar4) {
            uVar5 = uVar4;
          }
        }
      }
    }
  } while( true );
}


