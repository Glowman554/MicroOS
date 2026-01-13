#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <stddef.h>
#include <stdio.h>

cpu_registers_t* sys_vmode(cpu_registers_t* regs) {
	if (global_char_output_driver == NULL || global_char_output_driver->vmode == NULL) {
		abortf("sys_vmode");
	}

	regs->ebx = global_char_output_driver->vmode(global_char_output_driver);

	return regs;
}