#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>

cpu_registers_t* sys_vmode(cpu_registers_t* regs) {

	regs->ebx = global_char_output_driver->vmode(global_char_output_driver);

	return regs;
}