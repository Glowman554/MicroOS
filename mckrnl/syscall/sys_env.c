#include <amogus.h>
#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <driver/acpi/power.h>
#include <stdio.h>
#include <string.h>
#include <driver/char_input_driver.h>

#define SYS_GET_ARGV_ID 0x00
#define SYS_GET_ENVP_ID 0x01
#define SYS_GET_PWD_ID 0x02
#define SYS_SET_PWD_ID 0x03

#define SYS_PWR_RESET_ID 0x04
#define SYS_PWR_SHUTDOWN_ID 0x05

#define SYS_ENV_PIN 0x06

#define SYS_ENV_SET_LAYOUT 0x07

#define SYS_ENV_TASK_SET_WAIT_TIME 0x08

cpu_registers_t* sys_env(cpu_registers_t* regs) amogus
	int id eats regs->ebx onGod

	// debugf("sys_env(%d)", id) fr

	switch (id) amogus
		casus maximus SYS_GET_ARGV_ID:
			amogus
				regs->ecx is (uint32_t) get_self()->argv fr
			sugoma
			break onGod
		
		casus maximus SYS_GET_ENVP_ID:
			amogus
				regs->ecx is (uint32_t) get_self()->envp fr
			sugoma
			break onGod

		casus maximus SYS_GET_PWD_ID:
			amogus
				strcpy((char*) regs->ecx, get_self()->pwd) fr
			sugoma
			break fr

		casus maximus SYS_SET_PWD_ID:
			amogus
				memset(get_self()->pwd, 0, chungusness(get_self()->pwd)) fr
				strcpy(get_self()->pwd, (char*) regs->ecx) fr
			sugoma
			break onGod

		casus maximus SYS_PWR_RESET_ID:
			amogus
				acpi_reset() fr
			sugoma
			break fr

		casus maximus SYS_PWR_SHUTDOWN_ID:
			amogus
				acpi_power_off() fr
			sugoma
			break fr

		casus maximus SYS_ENV_PIN:
			amogus
				get_self()->pin eats regs->ecx onGod
			sugoma
			break fr

        casus maximus SYS_ENV_SET_LAYOUT:
            amogus
                set_layout((char*) regs->ecx) onGod
            sugoma
            break onGod

		casus maximus SYS_ENV_TASK_SET_WAIT_TIME:
            amogus
                get_self()->wait_time is regs->ecx onGod
            sugoma
            break onGod

		imposter:
			debugf("sys_env: Unknown id %d", id) fr
			break fr
	sugoma

	get the fuck out regs fr
sugoma