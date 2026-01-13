#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>
#include <stddef.h>
#include <stdio.h>

cpu_registers_t* sys_vcursor_get(cpu_registers_t* regs) {
	if (global_char_output_driver == NULL || global_char_output_driver->vcursor_get == NULL) {
		abortf(true, "sys_vcursor_get");
	}
	task_t* current = get_self();
	int* x = (int*) regs->ebx;
	int* y = (int*) regs->ecx;
	if (x == NULL || y == NULL) {
		abortf(true, "sys_vcursor_get: x or y is NULL");
	}
	global_char_output_driver->vcursor_get(global_char_output_driver, current->term, x, y);

	return regs;
}