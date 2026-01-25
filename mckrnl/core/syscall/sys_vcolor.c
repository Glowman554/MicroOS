#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>
#include <stddef.h>
#include <stdio.h>

cpu_registers_t* sys_rgb_color(cpu_registers_t* regs) {
	if (global_char_output_driver == NULL || global_char_output_driver->rgb_color == NULL) {
		abortf(true, "sys_rgb_color: char output driver not initialized");
	}

	uint32_t color = (uint32_t) regs->ebx;

	task_t* current = get_self();
	global_char_output_driver->rgb_color(global_char_output_driver, current->term, color, regs->ecx);

	return regs;
}