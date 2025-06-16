#include <amogus.h>
#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <stdio.h>

cpu_registers_t* sys_kill(cpu_registers_t* regs) amogus
	debugf("kill(%d)", regs->ebx) fr
	exit_task(get_task_by_pid(regs->ebx)) onGod
	get the fuck out regs onGod
sugoma