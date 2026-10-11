/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x10139 FUN_1000_0139
 * ev_9bfdbae5b6fa889661a850c31ea3556a179199ef9e5f8a03fcd32098a56fad65
 */

void FUN_1000_0139(void)

{
  char cVar1;
  char cVar2;
  char cVar3;
  char cVar4;
  char cVar5;
  char cVar6;
  byte local_3;
  
  FUN_1920_04df();
  cVar1 = FUN_1920_13a8(0x14);
  cVar2 = FUN_1920_13a8(0x14);
  cVar3 = FUN_1920_13a8(0x14);
  cVar4 = FUN_1920_13a8(0x1e);
  cVar5 = FUN_1920_13a8(0x1e);
  cVar6 = FUN_1920_13a8(0x1e);
  local_3 = 0xb0;
  while( true ) {
    FUN_18ac_0000((int)cVar3 + (int)((int)cVar6 * (local_3 - 0xb0)) / 7,
                  (int)cVar2 + (int)((int)cVar5 * (local_3 - 0xb0)) / 7,
                  (int)cVar1 + (int)((int)cVar4 * (local_3 - 0xb0)) / 7,local_3);
    if (local_3 == 0xb6) break;
    local_3 = local_3 + 1;
  }
  return;
}


