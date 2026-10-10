/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x19508 FUN_1920_0308
 * ev_d6a3b37344b7b33560daca05a78f352a1122da64bc77307a8c574a88cefc3a6e
 */

uint * __cdecl16near FUN_1920_0308(void)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  byte bVar5;
  int in_AX;
  undefined2 *puVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint *puVar10;
  int iVar11;
  int iVar12;
  int unaff_DS;
  bool bVar13;
  undefined4 uVar14;
  uint *puVar15;
  
  if (in_AX != 0) {
    *(int *)0xc6ea = in_AX;
    do {
      uVar14 = FUN_1920_0491();
      uVar9 = (uint)((ulong)uVar14 >> 0x10);
      uVar7 = (uint)uVar14;
      puVar10 = (uint *)0x1ae2;
      iVar12 = unaff_DS;
      while( true ) {
        iVar11 = iVar12;
        puVar15 = puVar10;
        puVar4 = *(uint **)puVar15;
        iVar12 = (int)((ulong)puVar4 >> 0x10);
        puVar10 = (uint *)puVar4;
        if (iVar12 == *(int *)0x1adc) break;
        uVar1 = puVar10[3];
        bVar13 = uVar9 == uVar1;
        if ((uVar9 <= uVar1) &&
           ((uVar9 < uVar1 || (bVar13 = uVar7 == puVar10[2], uVar7 <= puVar10[2])))) {
          uVar1 = puVar10[1];
          puVar6 = (undefined2 *)*puVar4;
          uVar8 = uVar1;
          if (!bVar13) {
            uVar2 = puVar10[2];
            uVar3 = puVar10[3];
            bVar5 = (byte)(uVar7 + (int)puVar10);
            uVar8 = uVar9 + iVar12 + (uint)(0xf < bVar5);
            puVar6 = (undefined2 *)
                     (CONCAT11((char)(uVar7 + (int)puVar10 >> 8),bVar5 - 0x10) & 0xff0f);
            *puVar6 = (undefined2 *)*puVar4;
            puVar6[1] = uVar1;
            puVar6[2] = uVar2 - uVar7 & 0xf;
            puVar6[3] = (uVar3 - uVar9) - (uint)(uVar2 < uVar7);
          }
          *puVar15 = (uint)puVar6;
          puVar15[1] = uVar8;
          return puVar10;
        }
      }
      bVar5 = (byte)(uVar7 + (int)puVar10);
      uVar9 = uVar9 + iVar12 + (uint)(0xf < bVar5);
      uVar7 = CONCAT11((char)(uVar7 + (int)puVar10 >> 8),bVar5 - 0x10) & 0xff0f;
      if ((uVar9 < *(uint *)0x1ae0) || ((uVar9 <= *(uint *)0x1ae0 && (uVar7 <= *(uint *)0x1ade)))) {
        *(uint *)0x1ada = uVar7;
        *(uint *)0x1adc = uVar9;
        *puVar15 = uVar7;
        puVar15[1] = uVar9;
        puVar15 = (uint *)0x0;
        (*(code *)*(undefined2 *)0x1aea)(0x1920,0,puVar10,iVar12);
        return puVar15;
      }
      puVar10 = (uint *)(*(code *)*(undefined2 *)0x1aea)(0x1920,*(undefined2 *)0xc6ea);
      if ((char)puVar10 == '\0') {
        return puVar10;
      }
    } while ((char)puVar10 != '\x01');
  }
  return (uint *)0x0;
}


