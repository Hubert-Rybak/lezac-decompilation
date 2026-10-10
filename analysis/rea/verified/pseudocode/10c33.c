/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x10c33 FUN_1000_0c33
 * ev_a9152f887a60ec7ebfd4b9ba0341ecb0afcfa6bc7c30b18d3dcfe764c331808b
 */

void __cdecl16near FUN_1000_0c33(void)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  int extraout_DX;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined4 uVar4;
  undefined2 local_e;
  undefined2 local_c;
  undefined2 local_a;
  undefined2 local_8;
  int local_6;
  undefined2 local_4;
  
  local_4 = 0x1000;
  local_6 = 0xc3e;
  FUN_1920_04df();
  FUN_1920_177b(0x7eda,unaff_DS);
  iVar3 = extraout_DX;
  iVar2 = FUN_1920_04a9();
  if ((*(int *)((uint)*(byte *)0x79b7 * 4 + 0x1a88) != iVar3) ||
     (*(int *)((uint)*(byte *)0x79b7 * 4 + 0x1a86) != iVar2)) {
    FUN_1920_174b(*(undefined2 *)((uint)*(byte *)0x79b7 * 4 + 0x1a86),
                  *(undefined2 *)((uint)*(byte *)0x79b7 * 4 + 0x1a88),0x7eda,unaff_DS);
    FUN_1920_04a9();
  }
  if (*(int *)0x6616 != 0 || *(int *)0x6618 != 0) {
    FUN_1920_0254(*(int *)0x78ba * 2 + 0x10,*(undefined2 *)0x6616,*(undefined2 *)0x6618);
    *(undefined2 *)0x6616 = 0;
    *(undefined2 *)0x6618 = 0;
  }
  if (*(int *)0x661a != 0 || *(int *)0x661c != 0) {
    FUN_1920_0254(*(int *)0x78ba + 0x10,*(undefined2 *)0x661a,*(undefined2 *)0x661c);
    *(undefined2 *)0x661a = 0;
    *(undefined2 *)0x661c = 0;
  }
  FUN_1920_16e3(0,0,2,&local_6,unaff_SS,0x7eda,unaff_DS);
  *(int *)0xc204 = local_6;
  FUN_1920_16e3(0,0,2,&local_6,unaff_SS,0x7eda,unaff_DS);
  *(int *)0x2096 = (local_6 + -0x15) * 8;
  *(int *)0x78ba = local_6 * *(int *)0xc204;
  FUN_1920_16e3(0,0,1,0x79b4,unaff_DS,0x7eda,unaff_DS);
  FUN_1920_16e3(0,0,2,0x2086,unaff_DS,0x7eda,unaff_DS);
  FUN_1920_16e3(0,0,1,0x79b3,unaff_DS,0x7eda,unaff_DS);
  uVar4 = FUN_1920_023f(*(int *)0x78ba + 0x10);
  *(undefined2 *)0xc1e0 = (int)uVar4;
  *(undefined2 *)0xc1e2 = (int)((ulong)uVar4 >> 0x10);
  uVar1 = *(undefined2 *)0xc1e2;
  *(undefined2 *)0x661a = *(undefined2 *)0xc1e0;
  *(undefined2 *)0x661c = uVar1;
  if ((int)*(undefined4 *)0xc1e0 != 0) {
    if ((uint)*(undefined4 *)0xc1e0 < 0x10) {
      uVar4 = *(undefined4 *)0xc1e0;
      *(undefined2 *)0xc1e0 = 0;
      *(int *)0xc1e2 = (int)((ulong)uVar4 >> 0x10) + 1;
    }
    else {
      FUN_1920_00e9();
    }
  }
  uVar4 = FUN_1920_023f(*(int *)0x78ba * 2 + 0x10);
  *(undefined2 *)0x6612 = (int)uVar4;
  *(undefined2 *)0x6614 = (int)((ulong)uVar4 >> 0x10);
  uVar1 = *(undefined2 *)0x6614;
  *(undefined2 *)0x6616 = *(undefined2 *)0x6612;
  *(undefined2 *)0x6618 = uVar1;
  if ((int)*(undefined4 *)0x6612 != 0) {
    if ((uint)*(undefined4 *)0x6612 < 0x10) {
      uVar4 = *(undefined4 *)0x6612;
      *(undefined2 *)0x6612 = 0;
      *(int *)0x6614 = (int)((ulong)uVar4 >> 0x10) + 1;
    }
    else {
      FUN_1920_00e9();
    }
  }
  *(undefined2 *)0xc1fe = (int)((ulong)*(undefined4 *)0xc1e0 >> 0x10);
  FUN_1920_16e3(0,0,2,&local_4,unaff_SS,0x7eda,unaff_DS);
  FUN_1920_16e3(0,0,local_4,(int)*(undefined4 *)0xc498,(int)((ulong)*(undefined4 *)0xc498 >> 0x10),
                0x7eda,unaff_DS);
  local_a = *(undefined2 *)0xc498;
  local_8 = *(undefined2 *)0xc49a;
  local_e = *(undefined2 *)0xc1e0;
  local_c = *(undefined2 *)0xc1e2;
  FUN_182d_0000(*(undefined2 *)0x78ba,&local_e,unaff_SS,local_4,&local_a,unaff_SS);
  FUN_1920_16e3(0,0,2,&local_4,unaff_SS,0x7eda,unaff_DS);
  FUN_1920_16e3(0,0,local_4,(int)*(undefined4 *)0xc498,(int)((ulong)*(undefined4 *)0xc498 >> 0x10),
                0x7eda,unaff_DS);
  *(int *)0x78ba = *(int *)0x78ba << 1;
  local_e = *(undefined2 *)0x6612;
  local_c = *(undefined2 *)0x6614;
  FUN_182d_0000(*(undefined2 *)0x78ba,&local_e,unaff_SS,local_4,&local_a,unaff_SS);
  *(uint *)0x78ba = *(uint *)0x78ba >> 1;
  FUN_1920_16e3(0,0,2,0x78c4,unaff_DS,0x7eda,unaff_DS);
  FUN_1920_16e3(0,0,2,0x78c6,unaff_DS,0x7eda,unaff_DS);
  FUN_1920_16e3(0,0,1,0x79a6,unaff_DS,0x7eda,unaff_DS);
  if (*(char *)0x79a6 != '\0') {
    FUN_1920_16e3(0,0,(uint)*(byte *)0x79a6 * 0x1e,0x74c6,unaff_DS,0x7eda,unaff_DS);
  }
  FUN_1920_16e3(0,0,1,0x79a5,unaff_DS,0x7eda,unaff_DS);
  if (*(char *)0x79a5 != '\0') {
    FUN_1920_16e3(0,0,(uint)*(byte *)0x79a5 * 7,0x771e,unaff_DS,0x7eda,unaff_DS);
  }
  FUN_1920_16e3(0,0,1,0x79a7,unaff_DS,0x7eda,unaff_DS);
  if (*(char *)0x79a7 != '\0') {
    FUN_1920_16e3(0,0,(uint)*(byte *)0x79a7 * 0xe,0x77ce,unaff_DS,0x7eda,unaff_DS);
  }
  iVar3 = FUN_1920_04a2();
  if (iVar3 != 0) {
    FUN_1000_00a3(1);
  }
  return;
}


