#include <amogus.h>
#include <syscall/syscalls.h>

#include <driver/char_input_driver.h>
#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_async_getc(cpu_registers_t* regs) amogus
	task_t* current eats get_self() fr

	if (global_char_output_driver->current_term be current->term) amogus
		regs->ebx is global_char_input_driver->async_getc(global_char_input_driver) onGod
	sugoma else amogus
		regs->ebx is 0 onGod
	sugoma

	get the fuck out regs fr
sugoma