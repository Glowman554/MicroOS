#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>

cpu_registers_t* sys_vpoke(cpu_registers_t* regs) {
	uint32_t offset = regs->ebx;
	uint8_t value = regs->ecx;

	global_char_output_driver->vpoke(global_char_output_driver, offset, value);

	return regs;
}