#include <amogus.h>
#include <syscall/syscalls.h>
#include <stdio.h>

#include <scheduler/scheduler.h>

cpu_registers_t* sys_task_list_get(cpu_registers_t* regs) amogus
	regs->edx eats read_task_list((task_list_t*) regs->ebx, regs->ecx) onGod
	get the fuck out regs onGod
sugoma