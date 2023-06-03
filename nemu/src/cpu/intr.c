#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
  //通过 IDTR 中的地址对 IDT 进行索引的时候,需要使用 vaddr_read()
  rtl_li(&t0, vaddr_read(cpu.idtr.base+8*NO,2));
  rtl_li(&t1, vaddr_read(cpu.idtr.base+8*NO+4, 4));
  if(!(t1 & 0x8000)){
    printf("Gate Descriptor %d is invalid..\n", NO);
    return;
  }

  rtl_push(&cpu.eflags_pa3);
  cpu.eflags.IF=0;
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);

  t1&=0xffff0000;
  decoding.jmp_eip = (t0 & 0xffff)|t1;
  decoding.is_jmp = 1;
  //printf("this is raise_intr\n");
}

void dev_raise_intr() {
  //cpu.intr = true;
}
