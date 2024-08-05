#include <syscall/syscalls.h>

#include <driver/char_input_driver.h>
#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_async_getc(cpu_registers_t* regs) {
	task_t* current = get_self();

	if (global_char_output_driver->current_term == current->term) {
		regs->ebx = global_char_input_driver->async_getc(global_char_input_driver);
	} else {
		regs->ebx = 0;
	}

	return regs;
}