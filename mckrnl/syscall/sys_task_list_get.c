#include <syscall/syscalls.h>
#include <stdio.h>
#include <stddef.h>

#include <scheduler/scheduler.h>

cpu_registers_t* sys_task_list_get(cpu_registers_t* regs) {
	task_list_t* task_list = (task_list_t*) regs->ebx;
	if (task_list == NULL) {
		abortf("sys_task_list_get: task_list is NULL");
	}

	regs->edx = read_task_list(task_list, regs->ecx);
	
	return regs;
}