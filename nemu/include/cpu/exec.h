#ifndef __CPU_EXEC_H__
#define __CPU_EXEC_H__

#include "nemu.h"

#define make_EHelper(name) void concat(exec_, name) (vaddr_t *eip)
typedef void (*EHelper) (vaddr_t *);

#include "cpu/decode.h"
// return the addr that eip points and make eip point to the next addr
static inline uint32_t instr_fetch(vaddr_t *eip, int len) {
  uint32_t instr = vaddr_read(*eip, len);
#ifdef DEBUG
  uint8_t *p_instr = (void *)&instr;
  int i;
  for (i = 0; i < len; i ++) {
    decoding.p += sprintf(decoding.p, "%02x ", p_instr[i]);
  }
#endif
  (*eip) += len;//更新 eip 的值指向下一条指令的地址
  return instr;
}

void rtl_setcc(rtlreg_t*, uint8_t);
//根据条件码子码获取对应的条件码名称
static inline const char* get_cc_name(int subcode) {
  static const char *cc_name[] = {
    "o", "no", "b", "nb",//溢出 低位设置
    "e", "ne", "be", "nbe",//等于 无符号数小于等于
    "s", "ns", "p", "np",//符号位设置 奇偶校验
    "l", "nl", "le", "nle"//低位设置 有符号数小于等于 
  };
  return cc_name[subcode];
}

#ifdef DEBUG
#define print_asm(...) Assert(snprintf(decoding.assembly, 80, __VA_ARGS__) < 80, "buffer overflow!")
#else
#define print_asm(...)
#endif

#define suffix_char(width) ((width) == 4 ? 'l' : ((width) == 1 ? 'b' : ((width) == 2 ? 'w' : '?')))

#define print_asm_template1(instr) \
  print_asm(str(instr) "%c %s", suffix_char(id_dest->width), id_dest->str)

#define print_asm_template2(instr) \
  print_asm(str(instr) "%c %s,%s", suffix_char(id_dest->width), id_src->str, id_dest->str)

#define print_asm_template3(instr) \
  print_asm(str(instr) "%c %s,%s,%s", suffix_char(id_dest->width), id_src->str, id_src2->str, id_dest->str)

#endif
