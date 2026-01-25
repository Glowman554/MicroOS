#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>
#include <stddef.h>
#include <stdio.h>

cpu_registers_t* sys_set_color(cpu_registers_t* regs) {
	if (global_char_output_driver == NULL || global_char_output_driver->set_color == NULL) {
		abortf(true, "sys_set_color: char output driver not initialized");
	}

	char* color = (char*) regs->ebx;
	if (color == NULL) {
		abortf(true, "sys_set_color: color pointer is NULL");
	}

	task_t* current = get_self();
	global_char_output_driver->set_color(global_char_output_driver, current->term, color, regs->ecx);

	return regs;
}