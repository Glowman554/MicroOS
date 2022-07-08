#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>

cpu_registers_t* sys_get_proc_info(cpu_registers_t* regs) {
	int pid = regs->ebx;

	regs->ecx = false;

	for (int i = 0; i < MAX_TASKS; i++) {
		if (tasks[i].active) {
			if (tasks[i].pid == pid) {
				regs->ecx = true;
			}
		}
	}

	return regs;
}