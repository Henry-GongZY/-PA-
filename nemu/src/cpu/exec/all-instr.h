#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

make_EHelper(call);			// control.c
make_EHelper(push); 		//data-mov.c
make_EHelper(sub); 			//arith.c
make_EHelper(xor); 			//logic.c
make_EHelper(pop);			//data-mov.c
make_EHelper(ret);  		//control.c

make_EHelper(add);
make_EHelper(or);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(and);
make_EHelper(cmp);
make_EHelper(jmp);
make_EHelper(jmp_rm);
