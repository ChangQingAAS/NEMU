#include "cpu/exec/helper.h"

make_helper(leave) {
	cpu.esp = cpu.ebp;
	cpu.ebp = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;

	print_asm("leave");
	return 1;
}
/*mov esp,ebp;//将ebp指向（ebp内部应当保存一个地址，所谓指向即这个地址对应的空间）的值赋给esp
pop ebp 

 leave指令将EBP寄存器的内容复制到ESP寄存器中，
以释放分配给该过程的所有堆栈空间。然后，它从堆栈恢复EBP寄存器的旧值。*/