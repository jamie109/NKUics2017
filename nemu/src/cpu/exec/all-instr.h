#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

// pa2 add----stage1
make_EHelper(call);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(ret);

// add.c
make_EHelper(lea);
make_EHelper(and);
make_EHelper(nop);
make_EHelper(add);
make_EHelper(cmp);
make_EHelper(setcc);
make_EHelper(movzx);
make_EHelper(movsx);
make_EHelper(test);
make_EHelper(jcc);
//add-longlong.c
make_EHelper(adc);
make_EHelper(or);
//bit.c
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(shr);
make_EHelper(inc);
make_EHelper(dec);
make_EHelper(not);
//fact.c
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
//goldbach.c
make_EHelper(cltd);
make_EHelper(cwtl);
//hello-str.c
make_EHelper(leave);
make_EHelper(call_rm);
make_EHelper(jmp_rm);





make_EHelper(sbb);
make_EHelper(div);
make_EHelper(idiv);
make_EHelper(neg);

make_EHelper(jmp);


//logical





make_EHelper(rol);