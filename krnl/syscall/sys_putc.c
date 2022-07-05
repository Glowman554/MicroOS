#include <syscall/syscalls.h>

#include <stdio.h>

cpu_registers_t* sys_putc(cpu_registers_t* regs) {
	char c = (char) regs->ebx;
	
	printf("%c", c);
	
	return regs;
}