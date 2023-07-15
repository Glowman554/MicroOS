#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <driver/acpi/power.h>
#include <stdio.h>
#include <string.h>

#define SYS_GET_ARGV_ID 0x00
#define SYS_GET_ENVP_ID 0x01
#define SYS_GET_PWD_ID 0x02
#define SYS_SET_PWD_ID 0x03

#define SYS_PWR_RESET_ID 0x04
#define SYS_PWR_SHUTDOWN_ID 0x05

#define SYS_ENV_PIN 0x06


cpu_registers_t* sys_env(cpu_registers_t* regs) {
	int id = regs->ebx;

	debugf("sys_env(%d)", id);

	switch (id) {
		case SYS_GET_ARGV_ID:
			{
				regs->ecx = (uint32_t) get_self()->argv;
			}
			break;
		
		case SYS_GET_ENVP_ID:
			{
				regs->ecx = (uint32_t) get_self()->envp;
			}
			break;

		case SYS_GET_PWD_ID:
			{
				strcpy((char*) regs->ecx, get_self()->pwd);
			}
			break;

		case SYS_SET_PWD_ID:
			{
				memset(get_self()->pwd, 0, sizeof(get_self()->pwd));
				strcpy(get_self()->pwd, (char*) regs->ecx);
			}
			break;

		case SYS_PWR_RESET_ID:
			{
				acpi_reset();
			}
			break;

		case SYS_PWR_SHUTDOWN_ID:
			{
				acpi_power_off();
			}
			break;

		case SYS_ENV_PIN:
			{
				get_self()->pin = regs->ecx;
			}
			break;

		default:
			debugf("sys_env: Unknown id %d", id);
			break;
	}

	return regs;
}