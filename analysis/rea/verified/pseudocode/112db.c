/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x112db FUN_1000_12db
 * ev_114f23fc21a9f330b61dfc218c4eb18743c4fab519cb72a4ca950afc8bc12b16
 */

void FUN_1000_12db(byte param_1,int param_2,int param_3)

{
  undefined2 uVar1;
  char *pcVar2;
  char cVar3;
  byte bVar4;
  undefined1 uVar5;
  char cVar6;
  uint uVar7;
  uint uVar8;
  char *pcVar9;
  int iVar10;
  undefined1 *puVar11;
  undefined1 *puVar12;
  undefined2 unaff_DS;
  byte *local_a;
  
  FUN_1920_04df();
  iVar10 = (uint)param_1 * 4;
  _local_a = (byte *)CONCAT22(unaff_DS,(byte *)(iVar10 + -0x3cde));
  pcVar9 = (char *)*(int *)(iVar10 + -0x3cdc);
  bVar4 = *_local_a;
  puVar11 = (undefined1 *)(param_3 + param_2 * 0x140);
  uVar8 = (uint)*(byte *)(iVar10 + -0x3cdd);
  iVar10 = (uint)*(byte *)0x79a3 << 8;
  uVar5 = *(undefined1 *)0x661e;
  uVar1 = *(undefined2 *)0xc1fa;
  do {
    uVar7 = (uint)bVar4;
    puVar12 = puVar11;
    do {
      pcVar2 = pcVar9;
      pcVar9 = pcVar9 + 1;
      cVar3 = *pcVar2;
      cVar6 = (char)((uint)iVar10 >> 8);
      iVar10 = CONCAT11(cVar6,cVar3);
      if (cVar3 == '\0') {
        iVar10 = CONCAT11(cVar6,cVar6);
        if (cVar6 != '\0') goto LAB_1000_135e;
      }
      else {
        if (cVar3 == '\x01') {
          iVar10 = CONCAT11(cVar6,uVar5);
        }
LAB_1000_135e:
        *puVar12 = (char)iVar10;
      }
      puVar12 = puVar12 + 1;
      uVar7 = uVar7 - 1;
    } while (uVar7 != 0);
    puVar11 = puVar11 + 0x140;
    uVar8 = uVar8 - 1;
    if (uVar8 == 0) {
      return;
    }
  } while( true );
}


