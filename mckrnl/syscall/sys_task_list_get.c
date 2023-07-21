#include <syscall/syscalls.h>
#include <stdio.h>

#include <scheduler/scheduler.h>

cpu_registers_t* sys_task_list_get(cpu_registers_t* regs) {
	regs->edx = read_task_list((task_list_t*) regs->ebx, regs->ecx);
	return regs;
}