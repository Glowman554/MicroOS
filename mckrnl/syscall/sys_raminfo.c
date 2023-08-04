#include <syscall/syscalls.h>

#include <memory/pmm.h>

cpu_registers_t* sys_raminfo(cpu_registers_t* regs) {
	regs->ebx = free_memory;
    regs->ecx = used_memory;

	return regs;
}