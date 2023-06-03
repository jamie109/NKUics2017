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
  // //获取门描述符
  // vaddr_t gate_addr=cpu.idtr.base+8*NO;
  // //P位校验
  // if (cpu.idtr.limit<0){
  //   assert(0);
  // }
  // //将eflags、cs、返回地址压栈
  // ;
  // rtl_push(&cpu.cs);
  // rtl_push(&cpu.eflags)
  // rtl_push(&ret_addr);
  // //组合中断处理程序入口点
  // uint32_t high,low;
  // low=vaddr_read(gate_addr,4)&0xffff;
  // high=vaddr_read(gate_addr+4,4)&0xffff0000;
  // //设置eip跳转
  // decoding.jmp_eip=high|low;
  // decoding.is_jmp=true;

}

void dev_raise_intr() {
  //cpu.intr = true;
}
