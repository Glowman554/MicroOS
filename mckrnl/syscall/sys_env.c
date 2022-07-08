#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <stdio.h>
#include <string.h>

#define SYS_GET_ARGV_ID 0x00
#define SYS_GET_ENVP_ID 0x01
#define SYS_GET_PWD_ID 0x02
#define SYS_SET_PWD_ID 0x03

cpu_registers_t* sys_env(cpu_registers_t* regs) {
	int id = regs->ebx;

	debugf("sys_env(%d)", id);

	switch (id) {
		case SYS_GET_ARGV_ID:
			{
				regs->ecx = (uint32_t) tasks[current_task].argv;
			}
			break;
		
		case SYS_GET_ENVP_ID:
			{
				regs->ecx = (uint32_t) tasks[current_task].envp;
			}
			break;

		case SYS_GET_PWD_ID:
			{
				strcpy((char*) regs->ecx, tasks[current_task].pwd);
			}
			break;

		case SYS_SET_PWD_ID:
			{
				memset(tasks[current_task].pwd, 0, sizeof(tasks[current_task].pwd));
				strcpy(tasks[current_task].pwd, (char*) regs->ecx);
			}
			break;

		default:
			debugf("sys_env: Unknown id %d", id);
			break;
	}

	return regs;
}