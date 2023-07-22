#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <stdio.h>

cpu_registers_t* sys_kill(cpu_registers_t* regs) {
	debugf("kill(%d)", regs->ebx);
	exit_task(get_task_by_pid(regs->ebx));
	return regs;
}