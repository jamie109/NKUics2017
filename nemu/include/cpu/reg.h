#ifndef __REG_H__
#define __REG_H__

#include "common.h"
// "anonymous enum" appends an integer value to the constants in each enum struct
// When using the anonymous enum constant, we can directly use the constant name to represent the corresponding value
// in the first enum R_EAX = 0 R_ECX = 1......when we use R_EAX later we know its value is 0
enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/****
 this is pa1 branch
****/

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */
// finished!!!
typedef struct {
  // EAX AX AH+AL use the same memory space
  union {//gpr[8] and "rtlreg_t eax, ecx, edx, ......" are the same thing(8 regs) so add another union
  union {
    uint32_t _32;
    uint16_t _16;
    uint8_t _8[2];
  } gpr[8];

  /* Do NOT change the order of the GPRs' definitions. */

  /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
   * in PA2 able to directly access these registers.
   */
  // add a struct to "eax, ecx, edx,..."  make it can use .eax to visit eax
  // else it will "error" like this "nemu: src/cpu/reg.c:34: reg_test: Assertion `sample[R_ECX] == cpu.ecx' failed."

  struct{
    rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;};
  };

  vaddr_t eip;
  // add for pa2
union{//add union for pa3
  struct {
      uint32_t CF:1;
      uint32_t one:1;
      uint32_t :4;
      uint32_t ZF:1;
      uint32_t SF:1;
      uint32_t :1;
      uint32_t IF:1;
      uint32_t :1;
      uint32_t OF:1;
      uint32_t :20;
  }eflags;
   uint32_t eflags_pa3;
};
  //pa3 add
  uint32_t cs;
  struct{
    uint32_t base;
    uint16_t limit;
  } idtr;
  //pa4 add
  uint32_t CR0;
  uint32_t CR3;
} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 8);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

static inline const char* reg_name(int index, int width) {
  assert(index >= 0 && index < 8);
  switch (width) {
    case 4: return regsl[index];
    case 1: return regsb[index];
    case 2: return regsw[index];
    default: assert(0);
  }
}

#endif
