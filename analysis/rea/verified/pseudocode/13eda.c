/* RAW GHIDRA PSEUDOCODE: not compilable reconstructed source.
 * 0x13eda FUN_1000_3eda
 * ev_a23beb03b6372e6f4d9b6277e7c206829308cb36796946b755665feee0cebd14
 */

void __cdecl16near FUN_1000_3eda(void)

{
  char cVar1;
  char cVar2;
  int iVar3;
  undefined2 unaff_DS;
  
  cVar1 = *(char *)0x78d2;
  iVar3 = 1;
  if (cVar1 < '\0') {
    iVar3 = -1;
  }
  cVar2 = *(char *)0x78d3 + cVar1;
  if (SCARRY1(*(char *)0x78d3,cVar1)) {
    cVar2 = cVar2 + -0x80;
    *(int *)0x2090 = *(int *)0x2090 + iVar3;
  }
  *(char *)0x78d3 = cVar2;
  cVar1 = *(char *)0x78d4;
  iVar3 = *(int *)0xc204;
  if (cVar1 < '\0') {
    iVar3 = -iVar3;
  }
  cVar2 = *(char *)0x78d5 + cVar1;
  if (SCARRY1(*(char *)0x78d5,cVar1)) {
    cVar2 = cVar2 + -0x80;
    *(int *)0x2090 = *(int *)0x2090 + iVar3;
  }
  *(char *)0x78d5 = cVar2;
  return;
}


