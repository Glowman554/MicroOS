#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <config.h>

cpu_registers_t* sys_set_term(cpu_registers_t* regs) {
	int pid = regs->ebx;
	int term = regs->ecx;

    if (term <= MAX_VTERM) {
        task_t* task = get_task_by_pid(pid);
        task->term = term;
    }

	return regs;
}