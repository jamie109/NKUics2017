#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

// pa2 add----stage1
make_EHelper(call);
//make_EHelper(nop);
make_EHelper(push);
make_EHelper(sub);
make_EHelper(jmp);
make_EHelper(ret);
make_EHelper(pop);