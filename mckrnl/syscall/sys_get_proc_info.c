#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <stddef.h>

cpu_registers_t* sys_get_proc_info(cpu_registers_t* regs) {
	int pid = regs->ebx;

	if (get_task_by_pid(pid) != NULL) {
		regs->ecx = true;
	} else {
		regs->ecx = false;
	}

	return regs;
}