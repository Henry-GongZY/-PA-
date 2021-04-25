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

make_EHelper(lea);
make_EHelper(and);
make_EHelper(nop);
make_EHelper(add);
make_EHelper(cmp);
make_EHelper(setcc);
make_EHelper(movzx);
make_EHelper(test);
make_EHelper(jcc);
make_EHelper(adc);
make_EHelper(or);
make_EHelper(shl);
make_EHelper(sar);
make_EHelper(shr);
make_EHelper(dec);
make_EHelper(inc);
make_EHelper(not);
make_EHelper(jmp);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(movsx);
make_EHelper(leave);
make_EHelper(call_rm);
make_EHelper(jmp_rm);
make_EHelper(sbb);
make_EHelper(cwtl);
make_EHelper(cltd);
make_EHelper(div);
make_EHelper(idiv);
