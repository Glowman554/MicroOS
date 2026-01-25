#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>

cpu_registers_t* sys_exit(cpu_registers_t* regs) {
	exit_task(get_self());
	
	return regs;
}