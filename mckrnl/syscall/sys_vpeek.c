#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_vpeek(cpu_registers_t* regs) {
	uint32_t offset = regs->ebx;
	uint8_t* value = (uint8_t*) regs->ecx;
	uint32_t range = regs->edx;

	if (value == NULL || !global_char_output_driver || !global_char_output_driver->vpeek) {
		abortf(true, "sys_vpeek");
	}

	task_t* current = get_self();
    global_char_output_driver->vpeek(global_char_output_driver, current->term, offset, value, range);
    
	return regs;
}