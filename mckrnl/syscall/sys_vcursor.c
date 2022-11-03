#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>

cpu_registers_t* sys_vcursor(cpu_registers_t* regs) {

	global_char_output_driver->vcursor(global_char_output_driver, regs->ebx, regs->ecx);

	return regs;
}