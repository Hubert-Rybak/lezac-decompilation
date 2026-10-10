/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x17783 entry
 * ev_b4cfdbad80d7ae5f6f0c628025594c3c324c6010db08a379ab1db23c5c3a2fd4
 */

/* WARNING: Stack frame is not setup normally: Input value of stackpointer is not used */
/* WARNING: This function may have set the stack pointer */

void entry(void)

{
  uint *puVar1;
  char *pcVar2;
  bool bVar3;
  code *pcVar4;
  undefined4 uVar5;
  undefined2 *puVar6;
  undefined1 uVar7;
  char cVar8;
  uint uVar9;
  undefined2 uVar10;
  int iVar11;
  undefined2 extraout_DX;
  uint in_BX;
  undefined1 *puVar12;
  undefined2 *puVar13;
  undefined2 *puVar14;
  undefined2 unaff_BP;
  int unaff_SI;
  undefined2 *puVar15;
  uint *puVar16;
  undefined2 uVar17;
  undefined2 unaff_DS;
  
  DAT_2000_b10e = 0x1000;
  DAT_2000_b10a = (undefined2 *)CONCAT22(0x7788,(undefined2 *)DAT_2000_b10a);
  FUN_1920_0000();
  DAT_2000_b10e = 0x1920;
  DAT_2000_b10a = (undefined2 *)CONCAT22(0x778d,(undefined2 *)DAT_2000_b10a);
  FUN_18ac_072d();
  DAT_2000_b10e = 0x18ac;
  DAT_2000_b10a = (undefined2 *)CONCAT22(0x7792,(undefined2 *)DAT_2000_b10a);
  FUN_184a_0000();
  DAT_2000_b10e = 0x184a;
  DAT_2000_b10a = (undefined2 *)CONCAT22(0x7797,(undefined2 *)DAT_2000_b10a);
  FUN_182d_0111();
  uVar17 = 0x1920;
  DAT_2000_b10a = (undefined2 *)0x182d77a2;
  DAT_2000_b10e = unaff_BP;
  FUN_1920_04df();
  puVar12 = (undefined1 *)0x3ff6;
  *(undefined2 *)0x6616 = 0;
  *(undefined2 *)0x6618 = 0;
  *(undefined2 *)0x661a = 0;
  *(undefined2 *)0x661c = 0;
  *(undefined2 *)0x2074 = 0;
  DAT_2000_b104._0_1_ = 0xbd;
  DAT_2000_b104._1_1_ = 0x77;
  FUN_1000_26e8();
  *(undefined1 *)0x2058 = 0x30;
LAB_1000_77c2:
  *(undefined2 *)(puVar12 + -2) = 0x77c5;
  FUN_1000_2361();
  if (*(char *)0x79b8 == '\0') {
    *(undefined2 *)(puVar12 + -2) = 0;
    *(undefined2 *)(puVar12 + -4) = 0x82c6;
    FUN_1000_00a3();
    uVar10 = DAT_2000_b10e;
    DAT_2000_b10a = (undefined2 *)CONCAT22(0x82ce,(undefined2 *)DAT_2000_b10a);
    DAT_2000_b10e = uVar17;
    cVar8 = FUN_1920_00e9();
    *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + cVar8;
    DAT_2000_b10a = (undefined2 *)0x192082db;
    DAT_2000_b10e = uVar10;
    FUN_1920_04df();
    DAT_2000_b0fe = 0;
    DAT_2000_b0fc = 0;
    DAT_2000_b10a._0_2_ = (undefined2 *)*piRam0002b11c;
    DAT_2000_b10a._2_2_ = ((int *)piRam0002b11c)[1];
    DAT_2000_b106 = (uint *)CONCAT22(((undefined2 *)puRam0002b116)[1],*puRam0002b116);
    do {
      DAT_2000_b100._1_1_ = *(byte *)((int)(undefined2 *)DAT_2000_b10a + DAT_2000_b0fe);
      DAT_2000_b102._1_1_ = *(undefined1 *)((int)(undefined2 *)DAT_2000_b10a + DAT_2000_b0fe + 1);
      DAT_2000_b102._0_1_ = *(undefined1 *)((int)(undefined2 *)DAT_2000_b10a + DAT_2000_b0fe + 2);
      DAT_2000_b0fe = DAT_2000_b0fe + 3;
      DAT_2000_b104._1_1_ = (DAT_2000_b100._1_1_ >> 4) + 1;
      DAT_2000_b0f6 = DAT_2000_b104._1_1_ + DAT_2000_b0fc;
      if (DAT_2000_b0fc <= DAT_2000_b0f6) {
        DAT_2000_b0fa = DAT_2000_b0fc;
        while (*(undefined1 *)((int)DAT_2000_b106 + DAT_2000_b0fa) = DAT_2000_b102._1_1_,
              DAT_2000_b0fa != DAT_2000_b0f6) {
          DAT_2000_b0fa = DAT_2000_b0fa + 1;
        }
      }
      DAT_2000_b0fc = DAT_2000_b0fc + DAT_2000_b104._1_1_;
      if (DAT_2000_b0fc < uRam0002b114) {
        iVar11 = (uint)DAT_2000_b100._1_1_ % 0x10 + 1;
        DAT_2000_b104._1_1_ = (byte)iVar11;
        DAT_2000_b0f6 = iVar11 + DAT_2000_b0fc;
        uVar9 = DAT_2000_b0fc;
        if (DAT_2000_b0fc <= DAT_2000_b0f6) {
          while (DAT_2000_b0fa = uVar9,
                *(undefined1 *)((int)DAT_2000_b106 + DAT_2000_b0fa) = (undefined1)DAT_2000_b102,
                DAT_2000_b0fa != DAT_2000_b0f6) {
            uVar9 = DAT_2000_b0fa + 1;
          }
        }
        DAT_2000_b0fc = DAT_2000_b0fc + DAT_2000_b104._1_1_;
      }
    } while (DAT_2000_b0fc < uRam0002b114);
    return;
  }
  *(undefined2 *)(puVar12 + -2) = 0x77d2;
  FUN_1000_2efd();
  *(undefined2 *)0x78c2 = 0;
  *(undefined1 *)0x7f5e = 0;
LAB_1000_77dc:
  *(undefined2 *)(puVar12 + -2) = 0x77df;
  FUN_1000_2adc();
  *(undefined2 *)(puVar12 + -2) = uVar17;
  uVar17 = 0x18ac;
  *(undefined2 *)(puVar12 + -4) = 0x77e4;
  FUN_18ac_0022();
  *(undefined2 *)0xc1f8 = *(undefined2 *)0xc1ec;
  *(int *)0xc1f6 = *(int *)0xc1f8 - *(int *)0xc1ec;
  *(undefined1 *)0x79a3 = 1;
  while( true ) {
    *(undefined2 *)(puVar12 + -2) = 0x78;
    *(undefined2 *)(puVar12 + -4) = uVar17;
    *(undefined2 *)(puVar12 + -6) = 0x7806;
    uVar7 = FUN_1920_13a8();
    *(undefined1 *)0x79a8 = uVar7;
    *(undefined2 *)(puVar12 + -2) = 0x14;
    *(undefined2 *)(puVar12 + -4) = 0x1920;
    *(undefined2 *)(puVar12 + -6) = 0x7810;
    cVar8 = FUN_1920_13a8();
    *(char *)0x79ab = *(char *)0x79a8 + '\x14' + cVar8;
    *(undefined2 *)(puVar12 + -2) = 0x32;
    *(undefined2 *)(puVar12 + -4) = 0x1920;
    *(undefined2 *)(puVar12 + -6) = 0x7826;
    cVar8 = FUN_1920_13a8();
    *(char *)0x79a4 = cVar8 + 'P';
    if (*(char *)0x79a3 == '\x01') {
      *(undefined1 *)0x79a8 = 0;
      *(undefined1 *)0x79ab = 0xa0;
      *(undefined1 *)0x79a4 = 0x82;
    }
    if (*(byte *)0x79a4 < 0xa1) {
      *(uint *)0x2074 = (uint)*(byte *)0x79a4;
      while( true ) {
        DAT_2000_b10a = (undefined2 *)(ulong)CONCAT12(*(byte *)0x79ab,(undefined2 *)DAT_2000_b10a);
        puVar6 = DAT_2000_b10a;
        DAT_2000_b10a._2_2_ = (uint)*(byte *)0x79ab;
        bVar3 = *(byte *)0x79a8 <= DAT_2000_b10a._2_2_;
        DAT_2000_b10a = puVar6;
        if (bVar3) {
          *(uint *)0x2072 = (uint)*(byte *)0x79a8;
          while( true ) {
            *(undefined1 *)
             ((int)*(undefined4 *)0xc498 + *(int *)0x2074 * *(int *)0xc1ec + *(int *)0x2072) = 0xb0;
            if (*(int *)0x2072 == DAT_2000_b10a._2_2_) break;
            *(int *)0x2072 = *(int *)0x2072 + 1;
          }
        }
        if (*(int *)0x2074 == 0xa0) break;
        *(int *)0x2074 = *(int *)0x2074 + 1;
      }
    }
    if (*(char *)0x79a3 != '\x01') {
      *(undefined2 *)0x2074 = 1;
      while( true ) {
        *(uint *)(puVar12 + -2) = 0xa0 - (uint)*(byte *)0x79a4;
        *(undefined2 *)(puVar12 + -4) = 0x1920;
        *(undefined2 *)(puVar12 + -6) = 0x78ba;
        iVar11 = FUN_1920_13a8();
        *(uint *)(puVar12 + -2) = ((uint)*(byte *)0x79a4 + iVar11) * *(int *)0xc1ec;
        *(uint *)(puVar12 + -4) = (uint)*(byte *)0x79ab - (uint)*(byte *)0x79a8;
        *(undefined2 *)(puVar12 + -6) = 0x1920;
        *(undefined2 *)(puVar12 + -8) = 0x78dc;
        iVar11 = FUN_1920_13a8();
        *(undefined1 *)
         ((int)*(undefined4 *)0xc498 + (uint)*(byte *)0x79a8 + iVar11 + *(int *)(puVar12 + -2)) =
             0x16;
        if (*(int *)0x2074 == 0x14) break;
        *(int *)0x2074 = *(int *)0x2074 + 1;
      }
    }
    uVar17 = 0x1920;
    if (*(char *)0x79a3 == '\n') break;
    *(char *)0x79a3 = *(char *)0x79a3 + '\x01';
  }
LAB_1000_7903:
  *(int *)0x78c2 = *(int *)0x78c2 + 1;
  uVar7 = 0x1e;
  if (*(uint *)0x78c2 % 0x1e == 0) {
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    *(undefined1 *)0x79ab = (char)((uint)extraout_DX >> 8);
    *(undefined1 *)0x79ac = (char)extraout_DX;
    *(undefined1 *)0x79aa = uVar7;
    *(uint *)(puVar12 + -2) = (uint)*(byte *)0x79a9;
    in_BX = (uint)*(byte *)0x79ac;
    *(int *)0x2074 =
         (((uint)*(byte *)0x79aa - (uint)*(byte *)0x79a4) * 6000 +
          ((uint)*(byte *)0x79ab - (uint)*(byte *)0x79a8) * 100 + in_BX) - *(int *)(puVar12 + -2);
    uVar10 = 0;
    *(undefined2 *)(puVar12 + -2) = uVar17;
    *(undefined2 *)(puVar12 + -4) = 0x7970;
    uVar17 = FUN_1920_0f0f();
    *(undefined2 *)(puVar12 + -2) = uVar10;
    *(uint *)(puVar12 + -4) = in_BX;
    *(undefined2 *)(puVar12 + -6) = uVar17;
    *(undefined2 *)(puVar12 + -8) = 0x1920;
    *(undefined2 *)(puVar12 + -10) = 0x797d;
    FUN_1920_0f0f();
    *(undefined2 *)(puVar12 + -2) = 0x1920;
    *(undefined2 *)(puVar12 + -4) = 0x7985;
    FUN_1920_0f01();
    unaff_SI = 0;
    *(undefined2 *)(puVar12 + -2) = 0x1920;
    *(undefined2 *)(puVar12 + -4) = 0x7992;
    FUN_1920_0efb();
    *(undefined2 *)(puVar12 + -2) = 0x1920;
    uVar17 = 0x1920;
    *(undefined2 *)(puVar12 + -4) = 0x7997;
    uVar7 = FUN_1920_0f13();
    *(undefined1 *)0x79b5 = uVar7;
    if ((0x7d < *(uint *)0x2074) && (*(int *)0x78cc != 0)) {
      if (*(uint *)0x78ca < *(uint *)0x78cc) {
        *(int *)0x78cc = *(int *)0x78cc - *(int *)0x78ca;
      }
      else {
        *(undefined2 *)0x78cc = 0;
      }
    }
    if (*(uint *)0x2074 < 0x78) {
      *(int *)0x78cc = *(int *)0x78cc + *(int *)0x78ca;
    }
    *(undefined1 *)0x79a8 = *(undefined1 *)0x79ab;
    *(undefined1 *)0x79a9 = *(undefined1 *)0x79ac;
    *(undefined1 *)0x79a4 = *(undefined1 *)0x79aa;
  }
  *(undefined2 *)(puVar12 + -2) = *(undefined2 *)0x78cc;
  *(undefined2 *)(puVar12 + -4) = uVar17;
  *(undefined2 *)(puVar12 + -6) = 0x79e8;
  FUN_184a_029c();
  if ((*(char *)0x79b5 != *(char *)0x79b6) || (*(int *)0x2088 != *(int *)0x208a)) {
    *(undefined2 *)(puVar12 + -2) = 0xcb0f;
    *(undefined2 *)(puVar12 + -4) = 0x7a00;
    FUN_1000_3184();
  }
  while( true ) {
    *(undefined2 *)(puVar12 + -2) = 0x184a;
    uVar17 = 0x184a;
    *(undefined2 *)(puVar12 + -4) = 0x7a05;
    uVar9 = FUN_184a_02fd();
    if ((char)uVar9 == '\0') break;
    *(undefined2 *)(puVar12 + -2) = 0x184a;
    *(undefined2 *)(puVar12 + -4) = 0x7a0e;
    uVar7 = FUN_184a_030f();
    *(undefined1 *)0x2058 = uVar7;
  }
  if (*(char *)0x79e6 != '\0') {
    *(undefined2 *)(puVar12 + -2) = *(undefined2 *)0xc21e;
    *(undefined2 *)(puVar12 + -4) = *(undefined2 *)0xc220;
    *(undefined2 *)(puVar12 + -6) = 0x7a25;
    FUN_1000_3587();
    *(undefined2 *)(puVar12 + -2) = 0x184a;
    *(undefined2 *)(puVar12 + -4) = 0x7a2a;
    FUN_18ac_00f4();
    *(undefined2 *)0xc1f4 = *(undefined2 *)0x79f0;
    *(undefined2 *)(puVar12 + -2) = 0x18ac;
    uVar17 = 0x18ac;
    *(undefined2 *)(puVar12 + -4) = 0x7a35;
    uVar9 = FUN_18ac_03c8();
  }
  if (*(char *)0x79e7 != '\0') {
    *(undefined2 *)(puVar12 + -2) = *(undefined2 *)0xc226;
    *(undefined2 *)(puVar12 + -4) = *(undefined2 *)0xc228;
    *(undefined2 *)(puVar12 + -6) = 0x7a47;
    FUN_1000_3587();
    *(undefined2 *)(puVar12 + -2) = uVar17;
    *(undefined2 *)(puVar12 + -4) = 0x7a4c;
    FUN_18ac_00f4();
    *(undefined2 *)0xc1f4 = *(undefined2 *)0x79f2;
    *(undefined2 *)(puVar12 + -2) = 0x18ac;
    uVar17 = 0x18ac;
    *(undefined2 *)(puVar12 + -4) = 0x7a57;
    uVar9 = FUN_18ac_03c8();
  }
  *(undefined1 *)0x79e8 = 0;
  *(undefined1 *)0x79e9 = 0;
  *(undefined1 *)0x79ae = 0;
  *(undefined1 *)0x79af = 0;
  if (*(char *)0x79a6 != '\0') {
    *(undefined2 *)0x2082 = 1;
    while (uVar9 = (uint)*(byte *)0x79a6, *(uint *)0x2082 <= uVar9) {
      DAT_2000_b10a =
           (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(*(int *)0x2082 * 0x1e + 0x74a8));
      pcVar2 = (char *)(*(int *)0x2082 * 0x1e + 0x74c3);
      *pcVar2 = *pcVar2 + -1;
      uVar10 = (undefined2)((ulong)DAT_2000_b10a >> 0x10);
      puVar15 = (undefined2 *)DAT_2000_b10a;
      if ((((*(char *)((int)puVar15 + 0x1b) == '\0') && (*(char *)(puVar15 + 5) != '\0')) &&
          (*(char *)((int)puVar15 + 9) != '\0')) && (*(char *)(puVar15 + 4) == '\x01')) {
        *(undefined1 *)((int)puVar15 + 0x1b) = *(undefined1 *)(puVar15 + 0xe);
        *(undefined1 *)0x79a3 =
             *(undefined1 *)
              ((uint)*(byte *)((uint)*(byte *)((int)(undefined2 *)DAT_2000_b10a + 0xb) * 2 + 0x80) *
               2 + 0x58);
        *(undefined2 *)(puVar12 + -2) = *DAT_2000_b10a;
        *(undefined2 *)(puVar12 + -4) = ((undefined2 *)DAT_2000_b10a)[1];
        *(undefined2 *)(puVar12 + -6) = 0;
        *(undefined2 *)(puVar12 + -8) = 0;
        *(uint *)(puVar12 + -10) = (uint)*(byte *)0x79a3;
        *(uint *)(puVar12 + -0xc) = (uint)*(byte *)((int)(undefined2 *)DAT_2000_b10a + 0xb);
        *(undefined2 *)(puVar12 + -0xe) = 0;
        *(uint *)(puVar12 + -0x10) = (uint)*(byte *)((undefined2 *)DAT_2000_b10a + 0xd);
        *(undefined2 *)(puVar12 + -0x12) = 0x7b2b;
        FUN_1000_2f9f();
        if (*(int *)0x2072 == 1) {
          pcVar2 = (char *)((int)(undefined2 *)DAT_2000_b10a + 9);
          *pcVar2 = *pcVar2 + -1;
          *(char *)((undefined2 *)DAT_2000_b10a + 5) =
               *(char *)((undefined2 *)DAT_2000_b10a + 5) + -1;
          DAT_2000_b106 = (uint *)CONCAT22(unaff_DS,(uint *)((uint)*(byte *)0x208d * 0x26 + 0x1bae))
          ;
          *(undefined2 *)(puVar12 + -2) = ((undefined2 *)DAT_2000_b10a)[7];
          *(undefined2 *)(puVar12 + -4) = uVar17;
          *(undefined2 *)(puVar12 + -6) = 0x7b61;
          iVar11 = FUN_1920_13a8();
          ((uint *)DAT_2000_b106)[7] = iVar11 + ((undefined2 *)DAT_2000_b10a)[6];
          *(undefined2 *)(puVar12 + -2) = ((undefined2 *)DAT_2000_b10a)[9];
          *(undefined2 *)(puVar12 + -4) = 0x1920;
          *(undefined2 *)(puVar12 + -6) = 0x7b7b;
          iVar11 = FUN_1920_13a8();
          ((uint *)DAT_2000_b106)[8] = iVar11 + ((undefined2 *)DAT_2000_b10a)[8];
          *(undefined2 *)(puVar12 + -2) = ((undefined2 *)DAT_2000_b10a)[0xb];
          *(undefined2 *)(puVar12 + -4) = 0x1920;
          *(undefined2 *)(puVar12 + -6) = 0x7b95;
          iVar11 = FUN_1920_13a8();
          ((uint *)DAT_2000_b106)[9] = iVar11 + ((undefined2 *)DAT_2000_b10a)[10];
          *(uint *)(puVar12 + -2) = (uint)*(byte *)((int)(undefined2 *)DAT_2000_b10a + 0x19);
          *(undefined2 *)(puVar12 + -4) = 0x1920;
          uVar17 = 0x1920;
          *(undefined2 *)(puVar12 + -6) = 0x7bb2;
          cVar8 = FUN_1920_13a8();
          *(char *)((uint *)DAT_2000_b106 + 0x12) =
               *(char *)((undefined2 *)DAT_2000_b10a + 0xc) + cVar8;
          *(undefined1 *)((int)(uint *)DAT_2000_b106 + 3) =
               *(undefined1 *)((uint)*(byte *)((int)(undefined2 *)DAT_2000_b10a + 0xb) * 2 + 0x80);
          *(undefined1 *)((uint *)DAT_2000_b106 + 2) =
               *(undefined1 *)((uint)*(byte *)((int)(undefined2 *)DAT_2000_b10a + 0xb) * 2 + 0x81);
          puVar16 = (uint *)DAT_2000_b106;
          *(int *)(puVar12 + -2) = (int)((ulong)DAT_2000_b106 >> 0x10);
          *(uint **)(puVar12 + -4) = puVar16 + 0xb;
          *(uint *)(puVar12 + -6) = (uint)*(byte *)0x79a3;
          *(uint *)(puVar12 + -8) =
               (uint)*(byte *)((uint)*(byte *)((uint)*(byte *)((int)(undefined2 *)DAT_2000_b10a +
                                                              0xb) * 2 + 0x80) * 2 + 0x59);
          *(uint *)(puVar12 + -10) = (uint)*(byte *)((int)(undefined2 *)DAT_2000_b10a + 0x1d);
          *(undefined2 *)(puVar12 + -0xc) = 1;
          *(undefined2 *)(puVar12 + -0xe) = 0x7c2c;
          FUN_1000_06ab();
          *(undefined1 *)((int)(uint *)DAT_2000_b106 + 0x25) = *(undefined1 *)0x2082;
        }
      }
      *(int *)0x2082 = *(int *)0x2082 + 1;
    }
  }
  *(undefined2 *)0x2082 = 1;
  while( true ) {
    if ((*(char *)(*(int *)0x2082 + 0x1b75) != '\0') &&
       (*(char *)(*(int *)0x2082 + 0x79e5) == '\x01')) {
      iVar11 = *(int *)(*(int *)0x2082 * 2 + 0x79f2) + 0x79;
      *(int *)(puVar12 + -2) = iVar11;
      *(undefined2 *)(puVar12 + -4) = 0xa3;
      *(uint *)(puVar12 + -6) = CONCAT11((char)((uint)iVar11 >> 8),*(undefined1 *)0x2082);
      *(undefined2 *)(puVar12 + -8) = 0x7c77;
      uVar9 = FUN_1000_326e();
    }
    iVar11 = *(int *)0x2082 * 0x26;
    DAT_2000_b10a = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar11 + 0x1b62));
    if ((*(char *)(iVar11 + 0x1b77) == '\x02') &&
       (*(int *)(iVar11 + 0x1b72) = *(int *)(iVar11 + 0x1b72) + -1,
       ((undefined2 *)DAT_2000_b10a)[8] == 0)) {
      *(char *)(*(int *)0x2082 + 0x79e9) = *(char *)(*(int *)0x2082 + 0x79e9) + -1;
      if (*(char *)(*(int *)0x2082 + 0x79e9) == -1) {
        *(undefined1 *)(*(int *)0x2082 + 0x79e5) = 0;
        *(char *)0x79b8 = *(char *)0x79b8 + -1;
        *(int *)0x78cc = *(int *)0x78cc + 6;
        if (*(char *)0x79b8 == '\x01') {
          *(int *)(puVar12 + -2) = (*(int *)0x2082 + -1) * 0xa0;
          *(undefined2 *)(puVar12 + -4) = 0;
          *(undefined2 *)(puVar12 + -6) = 0xa0;
          *(undefined2 *)(puVar12 + -8) = 0xa0;
          *(undefined2 *)(puVar12 + -10) = 0;
          *(undefined2 *)(puVar12 + -0xc) = 0x7ce9;
          FUN_1000_07c5();
        }
        uVar9 = (uint)*(byte *)(*(int *)0x2082 * 0x26 + 0x1b63);
        *(undefined2 *)(uVar9 * 8 + -0x3de2) = 10000;
        if (*(char *)0x79b8 == '\0') {
          *(undefined2 *)(puVar12 + -2) = 1;
          *(undefined2 *)(puVar12 + -4) = 0x7d0b;
          FUN_1000_1b14();
          goto LAB_1000_77c2;
        }
      }
      else {
        iVar11 = *(int *)0x2082;
        *(undefined2 *)(puVar12 + -2) = unaff_DS;
        *(int *)(puVar12 + -4) = iVar11 * 0x26 + 0x1b62;
        *(uint *)(puVar12 + -6) = CONCAT11((char)(uVar9 >> 8),*(undefined1 *)0x2082);
        *(undefined2 *)(puVar12 + -8) = 0x7d23;
        FUN_1000_056b();
        *(undefined2 *)(puVar12 + -2) = *(undefined2 *)0x2082;
        *(undefined2 *)(puVar12 + -4) = 0x7d2a;
        FUN_1000_29db();
        if (*(byte *)(*(int *)0x2082 * 4 + 0x1b68) < 100) {
          *(undefined1 *)(*(int *)0x2082 * 4 + 0x1b68) = 100;
        }
        if (*(byte *)(*(int *)0x2082 * 4 + 0x1b69) < 10) {
          *(undefined1 *)(*(int *)0x2082 * 4 + 0x1b69) = 10;
        }
        if (*(byte *)(*(int *)0x2082 * 4 + 0x1b6a) < 2) {
          *(undefined1 *)(*(int *)0x2082 * 4 + 0x1b6a) = 2;
        }
        *(undefined1 *)(*(int *)0x2082 + 0x79e5) = 2;
        iVar11 = (*(int *)0x2082 + -1) * 8;
        DAT_2000_b106 = (uint *)CONCAT22(unaff_DS,(uint *)(iVar11 + -0x3de2));
        uVar9 = *(uint *)0xc3c0;
        *(uint *)(iVar11 + -0x3ddc) = uVar9;
        if (*(char *)0x79ca == '\0') {
          *(undefined1 *)((uint *)DAT_2000_b106 + 2) = 1;
          *(undefined1 *)((int)(uint *)DAT_2000_b106 + 5) = 1;
        }
      }
    }
    if (*(int *)0x2082 == 1) {
      *(undefined1 *)0x79a3 = *(undefined1 *)0x1b7b;
    }
    else {
      *(undefined1 *)0x79a3 = *(undefined1 *)0x1b80;
    }
    uVar9 = uVar9 & 0xff00;
    if (*(char *)0x79ca == '\0') {
      *(undefined1 *)0x79a3 = 0;
      *(undefined1 *)0x79b9 = 0xe5;
    }
    if (*(char *)(*(int *)0x2082 + 0x79e5) == '\x02') {
      iVar11 = (uint)*(byte *)(*(int *)0x2082 * 0x26 + 0x1b63) * 8;
      DAT_2000_b106 = (uint *)CONCAT22(unaff_DS,(uint *)(iVar11 + -0x3de2));
      *(int *)0x2074 =
           (*DAT_2000_b106 >> 3) + ((*(int *)(iVar11 + -0x3de0) + 7U >> 3) + 1) * *(int *)0xc204;
      uVar10 = (undefined2)((ulong)DAT_2000_b106 >> 0x10);
      if (((*(char *)((int)*(undefined4 *)0xc1e0 + *(int *)0x2074) != '\0') &&
          (uVar9 = *(uint *)0x2074, *(byte *)((int)*(undefined4 *)0xc1e0 + uVar9) < 0x4d)) ||
         ((uVar9 = *(int *)0x2074 + 1, *(char *)((int)*(undefined4 *)0xc1e0 + uVar9) != '\0' &&
          ((uVar9 = *(int *)0x2074 + 1, *(byte *)((int)*(undefined4 *)0xc1e0 + uVar9) < 0x4d &&
           (0x18 < ((uint *)DAT_2000_b106)[1])))))) {
        puVar1 = (uint *)DAT_2000_b106 + 1;
        *puVar1 = *puVar1 - 1;
      }
      if (*(char *)0x79a3 == '\x01') {
        uVar10 = (undefined2)((ulong)DAT_2000_b10a >> 0x10);
        if (*(int *)0x2082 == 1) {
          *(undefined1 *)((int)(undefined2 *)DAT_2000_b10a + 0x15) = 0;
        }
        else {
          *(undefined1 *)((int)(undefined2 *)DAT_2000_b10a + 0x15) = 1;
        }
        *(undefined1 *)(*(int *)0x2082 + 0x79e5) = 1;
        *(undefined1 *)0x1b7b = 0;
        *(undefined1 *)0x1b80 = 0;
        *(undefined1 *)(*(int *)0x2082 * 0x26 + 0x1b86) = 100;
      }
    }
    if (*(int *)0x2082 == 2) break;
    *(int *)0x2082 = *(int *)0x2082 + 1;
  }
  if (*(char *)0x79f9 != '\0') {
    *(undefined2 *)(puVar12 + -2) = 0x7ec5;
    FUN_1000_432a();
  }
  *(undefined2 *)0x2082 = 1;
  while (*(uint *)0x2082 <= (uint)*(byte *)0x208d) {
    iVar11 = *(int *)0x2082;
    *(undefined2 *)(puVar12 + -2) = unaff_DS;
    *(int *)(puVar12 + -4) = iVar11 * 0x26 + 0x1bae;
    puVar13 = (undefined2 *)(puVar12 + -6);
    puVar12 = puVar12 + -6;
    *puVar13 = 0x7ee4;
    FUN_1000_6053();
    *(int *)0x2082 = *(int *)0x2082 + 1;
  }
  if ((*(char *)0x79e6 == '\x01') || (*(char *)0x79e7 == '\x01')) {
    *(undefined1 *)0x79b9 = 0;
  }
  else {
    *(char *)0x79b9 = *(char *)0x79b9 + '\x01';
    if (*(char *)0x79b9 == -0x1a) {
      *(undefined2 *)0x2082 = 1;
      while( true ) {
        if (*(char *)(*(int *)0x2082 + 0x79e5) == '\x02') {
          *(undefined1 *)(*(int *)0x2082 + 0x79e5) = 1;
        }
        if (*(int *)0x2082 == 2) break;
        *(int *)0x2082 = *(int *)0x2082 + 1;
      }
      goto LAB_1000_77dc;
    }
  }
  *(undefined2 *)0x2082 = 1;
  while( true ) {
    if (*(char *)(*(int *)0x2082 + 0x79e5) == '\x01') {
      iVar11 = *(int *)0x2082;
      *(undefined2 *)(puVar12 + -2) = unaff_DS;
      *(int *)(puVar12 + -4) = iVar11 * 0x26 + 0x1b62;
      puVar14 = (undefined2 *)(puVar12 + -6);
      puVar12 = puVar12 + -6;
      *puVar14 = 0x7f5c;
      FUN_1000_6053();
      if (*(char *)(*(int *)0x2082 * 0x26 + 0x1b77) != '\x02') {
        pcVar2 = (char *)(*(int *)0x2082 * 0x26 + 0x1b86);
        *pcVar2 = *pcVar2 - *(char *)(*(int *)0x2082 + 0x79e7);
      }
      if (*(char *)(*(int *)0x2082 + 0x79e7) != '\0') {
        *(undefined2 *)0x2074 = 0x2d;
        *(undefined1 *)0x799f = 4;
        *(undefined2 *)(puVar12 + -2) = 0x7f92;
        FUN_1000_165a();
      }
      *(uint *)0x2074 = (uint)*(byte *)(*(int *)0x2082 * 0x26 + 0x1b86);
      if (200 < *(uint *)0x2074) {
        *(uint *)(puVar12 + -2) = (uint)*(byte *)0x2082;
        *(undefined2 *)(puVar12 + -4) = 0x7faf;
        FUN_1000_30a3();
      }
      if ((uint)*(byte *)(*(int *)0x2082 + 0x79eb) != *(uint *)0x2074) {
        *(undefined1 *)(*(int *)0x2082 + 0x79eb) = *(undefined1 *)0x2074;
        *(int *)0x2072 = *(int *)(*(int *)0x2082 * 2 + 0x79f2) + 1;
        *(undefined2 *)(puVar12 + -2) = 0x7fdb;
        FUN_1000_568a();
      }
      if (*(int *)0x2082 == 1) {
        *(undefined2 *)0x2072 = 0;
        *(undefined2 *)0x78b6 = 0x785a;
        *(undefined2 *)0x78b8 = unaff_DS;
      }
      else {
        *(undefined2 *)0x2072 = *(undefined2 *)0x2084;
        *(undefined2 *)0x78b6 = 0x7888;
        *(undefined2 *)0x78b8 = unaff_DS;
      }
      if (*(byte *)((int)*(undefined4 *)0x78b6 + 0x2c) < 2) {
        if (*(char *)((int)*(undefined4 *)0x78b6 + 0x2c) == '\0') {
          uVar5 = *(undefined4 *)0x78b6;
          *(int *)(puVar12 + -2) = (int)((ulong)uVar5 >> 0x10);
          *(int *)(puVar12 + -4) = (int)uVar5;
          *(undefined2 *)(puVar12 + -6) = 0x8026;
          FUN_1000_11b0();
        }
        *(int *)0x2074 = *(int *)(*(int *)0x2082 * 2 + 0x79f2) + -0x2400;
        *(undefined2 *)(puVar12 + -2) = *(undefined2 *)0x2072;
        *(undefined2 *)(puVar12 + -4) = 0x803d;
        FUN_1000_1287();
        *(undefined2 *)(puVar12 + -2) = *(undefined2 *)0x2072;
        *(undefined2 *)(puVar12 + -4) = 0x8044;
        FUN_1000_1239();
      }
    }
    if (*(int *)0x2082 == 2) break;
    *(int *)0x2082 = *(int *)0x2082 + 1;
  }
  if ((*(int *)0x2076 != 0) || (199 < *(uint *)0x207e)) {
    *(undefined2 *)(puVar12 + -2) = 0x8060;
    FUN_1000_45fa();
  }
  if (*(int *)0x2080 != 0) {
    *(undefined2 *)(puVar12 + -2) = 0x806a;
    FUN_1000_5102();
  }
  if (*(int *)0x209c != 0) {
    *(int *)(puVar12 + -2) = *(int *)0x209a << 1;
    *(undefined2 *)(puVar12 + -4) = uVar17;
    uVar17 = 0x1920;
    *(undefined2 *)(puVar12 + -6) = 0x807c;
    uVar10 = FUN_1920_13a8();
    *(undefined2 *)0x2098 = uVar10;
    *(int *)0x209a = *(int *)0x209a + -1;
    if (*(int *)0x209a == 0) {
      *(undefined2 *)0x209c = 0;
      *(undefined2 *)0x2098 = 0;
    }
  }
  if (*(char *)0x79cb != '\0') {
    DAT_2000_b10a = (undefined2 *)(ulong)CONCAT12(*(char *)0x79cb,(undefined2 *)DAT_2000_b10a);
    if (*(char *)0x79cb != '\0') {
      *(undefined2 *)0x2082 = 1;
      while( true ) {
        *(undefined1 *)0x208c = 0;
        *(undefined1 *)0x79a3 = 1;
        while( true ) {
          if (*(char *)(*(int *)0x2082 * 3 + (uint)*(byte *)0x79a3 + 0x79ce) !=
              *(char *)(*(int *)0x2082 * 3 + (uint)*(byte *)0x79a3 + 0x79d8)) {
            *(undefined1 *)0x208c = 1;
            if (*(byte *)(*(int *)0x2082 * 3 + (uint)*(byte *)0x79a3 + 0x79d8) <
                *(byte *)(*(int *)0x2082 * 3 + (uint)*(byte *)0x79a3 + 0x79ce)) {
              pcVar2 = (char *)(*(int *)0x2082 * 3 + (uint)*(byte *)0x79a3 + 0x79ce);
              *pcVar2 = *pcVar2 + -2;
            }
            else {
              pcVar2 = (char *)(*(int *)0x2082 * 3 + (uint)*(byte *)0x79a3 + 0x79ce);
              *pcVar2 = *pcVar2 + '\x02';
            }
          }
          if (*(char *)0x79a3 == '\x03') break;
          *(char *)0x79a3 = *(char *)0x79a3 + '\x01';
        }
        if ((*(char *)0x208c == '\0') && ((uint)*(byte *)0x79cb == *(uint *)0x2082)) {
          *(char *)0x79cb = *(char *)0x79cb + -1;
        }
        *(uint *)(puVar12 + -2) = (uint)*(byte *)(*(int *)0x2082 + 0x79cd);
        *(uint *)(puVar12 + -4) = (uint)*(byte *)(*(int *)0x2082 * 3 + 0x79cf);
        *(uint *)(puVar12 + -6) = (uint)*(byte *)(*(int *)0x2082 * 3 + 0x79d0);
        unaff_SI = *(int *)0x2082;
        *(uint *)(puVar12 + -8) = (uint)*(byte *)(unaff_SI * 3 + 0x79d1);
        *(undefined2 *)(puVar12 + -10) = uVar17;
        uVar17 = 0x18ac;
        *(undefined2 *)(puVar12 + -0xc) = 0x81b4;
        FUN_18ac_0000();
        if (*(int *)0x2082 == DAT_2000_b10a._2_2_) break;
        *(int *)0x2082 = *(int *)0x2082 + 1;
      }
    }
  }
  if (*(char *)0x79b2 != '\0') {
    *(char *)0x79b2 = *(char *)0x79b2 + -1;
    *(undefined2 *)(puVar12 + -2) = 0x81cd;
    FUN_1000_3f27();
  }
  if (*(uint *)0x78c2 % 5 == 0) {
    *(undefined2 *)(puVar12 + -2) = 0x81df;
    FUN_1000_079d();
    *(char *)0x79ad = *(char *)0x79ad + '\a';
    if (0x3f < *(byte *)0x79ad) {
      *(undefined1 *)0x79ad = 0x14;
    }
  }
  if (*(char *)0x2058 == 's') {
    *(bool *)0xc49c = *(char *)0xc49c == '\0';
    *(undefined1 *)0x2058 = 0x39;
  }
  if (((*(char *)0x2058 == 'e') && (0x15 < *(uint *)0xc1ea)) && (*(char *)0x79b8 == '\x01')) {
    *(undefined2 *)(puVar12 + -2) = 0;
    *(undefined2 *)(puVar12 + -4) = 0;
    *(undefined2 *)(puVar12 + -6) = 0x140;
    *(undefined2 *)(puVar12 + -8) = 0xa0;
    *(undefined2 *)(puVar12 + -10) = 0;
    *(undefined2 *)(puVar12 + -0xc) = 0x8233;
    FUN_1000_07c5();
    *(int *)(puVar12 + -2) = *(int *)0xc1ea + -1;
    *(undefined2 *)(puVar12 + -4) = 0x823b;
    FUN_1000_0838();
    *(undefined1 *)0x2058 = 0x39;
    *(undefined2 *)0x79ee = *(undefined2 *)0xc1ea;
    *(undefined2 *)(puVar12 + -2) = 0x8249;
    FUN_1000_2959();
  }
  if (((*(char *)0x2058 == 'r') && (*(uint *)0xc1ea < 0x28)) && (*(char *)0x79b8 == '\x01')) {
    *(undefined2 *)(puVar12 + -2) = 0;
    *(undefined2 *)(puVar12 + -4) = 0;
    *(undefined2 *)(puVar12 + -6) = 0x140;
    *(undefined2 *)(puVar12 + -8) = 0xa0;
    *(undefined2 *)(puVar12 + -10) = 0;
    *(undefined2 *)(puVar12 + -0xc) = 0x826d;
    FUN_1000_07c5();
    *(int *)(puVar12 + -2) = *(int *)0xc1ea + 1;
    *(undefined2 *)(puVar12 + -4) = 0x8275;
    FUN_1000_0838();
    *(undefined1 *)0x2058 = 0x39;
    *(undefined2 *)0x79ee = *(undefined2 *)0xc1ea;
    *(undefined2 *)(puVar12 + -2) = 0x8283;
    FUN_1000_2959();
  }
  if (((*(char *)0x79c5 != '\0') && (*(char *)0x79c6 != '\0')) && (*(int *)0x2080 == 0))
  goto code_r0x00018298;
  if (*(char *)0x2058 == '\x1b') {
    *(undefined2 *)(puVar12 + -2) = 1;
    *(undefined2 *)(puVar12 + -4) = 0x82be;
    FUN_1000_1b14();
    goto LAB_1000_77c2;
  }
  goto LAB_1000_7903;
code_r0x00018298:
  *(undefined2 *)(puVar12 + -2) = 0x829b;
  FUN_1000_1d61();
  if (7 < *(byte *)0x79b7) goto LAB_1000_82a7;
  goto LAB_1000_77dc;
LAB_1000_82a7:
  *(undefined2 *)(puVar12 + -2) = 2;
  *(undefined2 *)(puVar12 + -4) = 0x82ac;
  FUN_1000_1b14();
  goto LAB_1000_77c2;
}


