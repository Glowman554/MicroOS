#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <stdio.h>
#include <stddef.h>

cpu_registers_t* sys_kill(cpu_registers_t* regs) {
	debugf("kill(%d)", regs->ebx);
	task_t* task = get_task_by_pid(regs->ebx);
	if (task == NULL) {
		abortf("sys_kill: no such pid %d", regs->ebx);
	}
	exit_task(task);
	return regs;
}