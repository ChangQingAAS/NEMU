// call-template.h
#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
         // push(ip)
         // read operand address
         // eip += relative address
}
make_instr_helper(i)

// make_helper(concat(call_i_, SUFFIX)) {
// 	int len = concat(decode_i_, SUFFIX)(cpu.eip + 1);
//     reg_l(R_ESP) -= DATA_BYTE;
//     swaddr_write(reg_l(R_ESP), 4, cpu.eip + len + 1);
//     DATA_TYPE_S imm = op_src -> val;
//     print_asm("call\t%x",cpu.eip + 1 + len + imm);
//     cpu.eip += imm;
//     return len + 1;
// } 



// make_instr_helper(i)

#include "cpu/exec/template-end.h"