#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>

cpu_registers_t* sys_set_color(cpu_registers_t* regs) {

	global_char_output_driver->set_color(global_char_output_driver, (char*) regs->ebx, regs->ecx);

	return regs;
}