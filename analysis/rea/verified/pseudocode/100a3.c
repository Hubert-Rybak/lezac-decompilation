/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x100a3 FUN_1000_00a3
 * ev_fd376c93dc864b68c206bec6fb62a5860bb657b388f61b194a36822a75398cc1
 */

void FUN_1000_00a3(char param_1)

{
  code *pcVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  FUN_1920_04df();
  FUN_183f_0093(*(undefined2 *)0x1b60,*(undefined2 *)0x1b62,9);
  FUN_183f_0093(*(undefined2 *)0x1b64,*(undefined2 *)0x1b66,0x1c);
  FUN_184a_02f6();
  out(0x60,0xf3);
  FUN_184a_029c(200);
  out(0x60,0x47);
  pcVar1 = (code *)swi(0x10);
  (*pcVar1)();
  if (param_1 == '\x01') {
    uVar2 = 0xc59e;
    FUN_1920_08d0(0,0x68,0x184a);
    FUN_1920_084a(uVar2);
    FUN_1920_04a9();
  }
  else if (param_1 == '\x02') {
    uVar2 = 0xc59e;
    FUN_1920_08d0(0,0x90,0x184a);
    FUN_1920_084a(uVar2);
    FUN_1920_04a9();
  }
  *(undefined1 *)0x7f5c = *(undefined1 *)0x79c9;
  FUN_1920_00e9();
  return;
}


