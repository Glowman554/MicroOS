#include <stdint.h>
#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>

cpu_registers_t* sys_vpoke(cpu_registers_t* regs) {
	uint32_t offset = regs->ebx;
	uint8_t* value = (uint8_t*) regs->ecx;
	uint32_t range = regs->edx;

	for (uint32_t i = 0; i < range; i++) {
		global_char_output_driver->vpoke(global_char_output_driver, offset + i, value[i]);
	}

	return regs;
}