#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

// pa2 add----stage1
make_EHelper(call);
//make_EHelper(nop);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(ret);
// add.c
make_EHelper(lea);

make_EHelper(call_rm);
make_EHelper(jmp_rm);
make_EHelper(inc);
make_EHelper(dec);

make_EHelper(add);
make_EHelper(adc);
make_EHelper(cmp);


make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(sbb);
make_EHelper(div);
make_EHelper(idiv);
make_EHelper(neg);

make_EHelper(jmp);


//logical


make_EHelper(and);
make_EHelper(or);
make_EHelper(not);
make_EHelper(setcc);
make_EHelper(test);
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(shr);
make_EHelper(rol);