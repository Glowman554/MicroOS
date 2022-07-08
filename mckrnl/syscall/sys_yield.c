#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <stddef.h>

cpu_registers_t* sys_yield(cpu_registers_t* regs) {
	return schedule(regs, NULL);
}