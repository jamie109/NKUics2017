#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  //printf("exec_mov end\n");
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  rtl_push(&id_dest->val);
  //printf("exec_push end\n");
  print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
  rtl_pop(&t2);
  operand_write(id_dest,&t2);
  //printf("exec_pop end\n");
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO();
  t0 = cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&t0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);
  printf("pusha end\n");
  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);
  printf("popa end\n");
  print_asm("popa");
}

make_EHelper(leave) {
  //TODO();
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);
  //printf("exec_leave end\n");
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    // TODO();
    rtl_lr_w(&t0,R_AX);
    rtl_sext(&t0,&t0,2);
    rtl_sari(&t0,&t0,31);
    rtl_sr_w(R_DX,&t0);
  }
  else {
    // TODO();
    rtl_sari(&cpu.edx,&cpu.eax,31);
  }
  //printf("exec_cltd end\n");
  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    // TODO();
    rtl_sext(&t0,&cpu.eax,1);
    cpu.eax=(cpu.eax&0xffff0000)|(t0&0xffff);
  }
  else {
    // TODO();
    rtl_sext(&t0,&cpu.eax,2);
    cpu.eax=t0;
  }
  //printf("exec_cwtl end\n");
  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  //printf("exec_movsx end\n");
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
  //printf("exec_movzx end\n");
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  //printf("exec_lae end\n");
  print_asm_template2(lea);
}
