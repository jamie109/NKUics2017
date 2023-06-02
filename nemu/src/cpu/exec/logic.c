#include "cpu/exec.h"

make_EHelper(test) {
  //TODO();
  rtl_and(&t2,&id_dest->val,&id_src->val);
  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  printf("exec_test end\n");
  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  rtl_and(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  rtl_update_ZFSF(&t2,id_dest->width);
  printf("exec_and end\n");
  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();
  rtl_xor(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  printf("exec_xor end\n");
  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtl_or(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  printf("exec_or end\n");
  print_asm_template2(or);
}

make_EHelper(sar) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  printf("exec_setcc start\n");
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);
  printf("exec_setcc end\n");
  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}
