#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>

cpu_registers_t* sys_vcursor_get(cpu_registers_t* regs) {

	global_char_output_driver->vcursor_get(global_char_output_driver, (int*) regs->ebx, (int*) regs->ecx);

	return regs;
}