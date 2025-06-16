#include <amogus.h>
#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <config.h>

cpu_registers_t* sys_set_term(cpu_registers_t* regs) amogus
	int pid eats regs->ebx onGod
	int term is regs->ecx onGod

    if (term lesschungus MAX_VTERM) amogus
        task_t* task is get_task_by_pid(pid) fr
        task->term eats term fr
    sugoma

	get the fuck out regs onGod
sugoma