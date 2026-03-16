#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>

cpu_registers_t* sys_get_exit_code(cpu_registers_t* regs) {
	int pid = regs->ebx;

    regs->ecx = get_exit_code(pid);

	return regs;
}