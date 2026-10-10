/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x18bb4 FUN_18ac_00f4
 * ev_cc7e63c1f168363e0d5bf2924df01dfa541d7a9ed803e7362daa2f2d51a20a20
 */

void __cdecl16far FUN_18ac_00f4(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  byte *pbVar3;
  char *pcVar4;
  undefined2 *puVar5;
  uint *puVar6;
  undefined2 *puVar7;
  uint *puVar8;
  undefined4 uVar9;
  uint *puVar10;
  uint uVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  byte *pbVar18;
  byte *pbVar19;
  char *pcVar20;
  undefined2 *puVar21;
  undefined2 *puVar22;
  uint *puVar23;
  char *pcVar24;
  undefined2 unaff_DS;
  
  if (*(char *)0xc49c == '\x01') {
    uVar9 = *(undefined4 *)0xc498;
    puVar21 = (undefined2 *)
              ((int)uVar9 + (uint)(*(int *)0xc216 + *(int *)0xc20a) / 4 +
              ((uint)(*(int *)0xc218 + *(int *)0xc20c) >> 3) * *(int *)0xc1f8);
    uVar1 = *(undefined2 *)0xc212;
    puVar22 = (undefined2 *)(*(int *)0xc214 + *(int *)0xc20a + *(int *)0xc20c * *(int *)0xc1ec);
    uVar17 = *(uint *)0xc1ec;
    iVar12 = *(int *)0xc1ee;
    iVar13 = *(int *)0xc1f6;
    uVar11 = uVar17 >> 1;
    do {
      for (; uVar11 != 0; uVar11 = uVar11 - 1) {
        puVar7 = puVar22;
        puVar22 = puVar22 + 1;
        puVar5 = puVar21;
        puVar21 = puVar21 + 1;
        *puVar7 = *puVar5;
      }
      puVar21 = (undefined2 *)((int)puVar21 + iVar13);
      iVar12 = iVar12 + -1;
      uVar11 = uVar17 >> 1;
    } while (iVar12 != 0);
  }
  puVar23 = (uint *)*(undefined2 *)0xc214;
  uVar1 = *(undefined2 *)0xc212;
  pbVar18 = (byte *)*(undefined2 *)0xc1f0;
  iVar12 = 0x14;
  if (*(char *)0xc49c == '\x01') {
    do {
      *(undefined2 *)0xc210 = *(undefined2 *)0xc208;
      iVar13 = 8;
      do {
        iVar15 = *(int *)0xc1ea;
        pbVar19 = pbVar18;
        do {
          pbVar3 = pbVar19;
          pbVar19 = pbVar19 + 1;
          puVar10 = (uint *)((uint)*pbVar3 * 0x40 + *(int *)0xc210);
          iVar14 = 4;
          do {
            *puVar23 = *puVar23 & puVar10[0x109b];
            puVar6 = puVar10;
            puVar10 = puVar10 + 1;
            *puVar23 = *puVar23 | *puVar6;
            puVar23 = puVar23 + 1;
            iVar14 = iVar14 + -1;
          } while (iVar14 != 0);
          iVar15 = iVar15 + -1;
        } while (iVar15 != 0);
        *(int *)0xc210 = *(int *)0xc210 + 8;
        iVar13 = iVar13 + -1;
      } while (iVar13 != 0);
      pbVar18 = pbVar18 + *(int *)0xc204;
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
  }
  else {
    do {
      *(undefined2 *)0xc210 = *(undefined2 *)0xc208;
      iVar13 = 8;
      do {
        iVar15 = *(int *)0xc1ea;
        pbVar19 = pbVar18;
        do {
          pbVar3 = pbVar19;
          pbVar19 = pbVar19 + 1;
          puVar10 = (uint *)((uint)*pbVar3 * 0x40 + *(int *)0xc210);
          for (iVar14 = 4; iVar14 != 0; iVar14 = iVar14 + -1) {
            puVar8 = puVar23;
            puVar23 = puVar23 + 1;
            puVar6 = puVar10;
            puVar10 = puVar10 + 1;
            *puVar8 = *puVar6;
          }
          iVar15 = iVar15 + -1;
        } while (iVar15 != 0);
        *(int *)0xc210 = *(int *)0xc210 + 8;
        iVar13 = iVar13 + -1;
      } while (iVar13 != 0);
      pbVar18 = pbVar18 + *(int *)0xc204;
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
  }
  uVar17 = (uint)*(byte *)0xc496;
  if (uVar17 != 0) {
    iVar12 = 0;
    do {
      *(uint *)0xc202 = (uint)*(byte *)(iVar12 + -0x3dde);
      *(uint *)0xc200 = (uint)*(byte *)(iVar12 + -0x3ddd);
      *(int *)0xc21a = *(int *)(iVar12 + -0x3de2) - *(int *)0xc216;
      iVar13 = *(int *)(iVar12 + -0x3de0) - *(int *)0xc218;
      *(int *)0xc21c = iVar13;
      if ((((iVar13 < *(int *)0xc1ee) && (0 < iVar13 + *(int *)0xc200)) &&
          (*(int *)0xc21a < *(int *)0xc1ec)) && (0 < *(int *)0xc21a + *(int *)0xc202)) {
        if (((*(uint *)0xc21c < 0x8000) &&
            ((int)(*(uint *)0xc21c + *(int *)0xc200 + -1) < *(int *)0xc1ee)) &&
           ((-1 < *(int *)0xc21a && (*(int *)0xc21a + *(int *)0xc202 + -1 < *(int *)0xc1ec)))) {
          uVar1 = *(undefined2 *)0xc212;
          pcVar24 = (char *)(*(int *)0xc214 + *(int *)0xc21c * *(int *)0xc1ec + *(int *)0xc21a);
          pcVar20 = (char *)*(undefined2 *)(iVar12 + -0x3ddc);
          *(int *)0xc1e8 = *(int *)0xc1ec - *(int *)0xc202;
          iVar13 = *(int *)0xc200;
          iVar15 = *(int *)0xc202;
          iVar14 = *(int *)0xc1e8;
          uVar2 = *(undefined2 *)0xc1fa;
          iVar16 = iVar15;
          do {
            do {
              pcVar4 = pcVar20;
              pcVar20 = pcVar20 + 1;
              if (*pcVar4 != '\0') {
                *pcVar24 = *pcVar4;
              }
              pcVar24 = pcVar24 + 1;
              iVar16 = iVar16 + -1;
            } while (iVar16 != 0);
            pcVar24 = pcVar24 + iVar14;
            iVar13 = iVar13 + -1;
            iVar16 = iVar15;
          } while (iVar13 != 0);
        }
        else {
          *(undefined2 *)0xc210 = 0;
          uVar1 = *(undefined2 *)0xc212;
          iVar15 = *(int *)0xc214;
          pcVar20 = (char *)*(undefined2 *)(iVar12 + -0x3ddc);
          iVar13 = *(int *)0xc21c;
          if (iVar13 < 0) {
            iVar14 = *(int *)0xc202;
            *(int *)0xc200 = *(int *)0xc200 + iVar13;
            pcVar20 = pcVar20 + -iVar13 * iVar14;
          }
          iVar13 = *(int *)0xc21a;
          if (iVar13 < 0) {
            pcVar20 = pcVar20 + -iVar13;
            *(int *)0xc210 = -iVar13;
          }
          else if (*(int *)0xc1ec < iVar13 + *(int *)0xc202) {
            *(int *)0xc210 = (iVar13 + *(int *)0xc202) - *(int *)0xc1ec;
          }
          iVar13 = (*(int *)0xc21c + *(int *)0xc200) - *(int *)0xc1ee;
          if (-1 < iVar13) {
            *(int *)0xc200 = *(int *)0xc200 - iVar13;
          }
          *(int *)0xc202 = *(int *)0xc202 - *(int *)0xc210;
          if (*(int *)0xc21c < 0) {
            *(undefined2 *)0xc21c = 0;
          }
          if (*(int *)0xc21a < 0) {
            *(undefined2 *)0xc21a = 0;
          }
          pcVar24 = (char *)(iVar15 + *(int *)0xc21c * *(int *)0xc1ec + *(int *)0xc21a);
          *(int *)0xc1e8 = *(int *)0xc1ec - *(int *)0xc202;
          iVar13 = *(int *)0xc200;
          do {
            iVar15 = *(int *)0xc202;
            uVar2 = *(undefined2 *)0xc1fa;
            do {
              pcVar4 = pcVar20;
              pcVar20 = pcVar20 + 1;
              if (*pcVar4 != '\0') {
                *pcVar24 = *pcVar4;
              }
              pcVar24 = pcVar24 + 1;
              iVar15 = iVar15 + -1;
            } while (iVar15 != 0);
            pcVar24 = pcVar24 + *(int *)0xc1e8;
            pcVar20 = pcVar20 + *(int *)0xc210;
            iVar13 = iVar13 + -1;
          } while (iVar13 != 0);
        }
      }
      iVar12 = iVar12 + 8;
      uVar17 = uVar17 - 1;
    } while (uVar17 != 0);
  }
  return;
}


