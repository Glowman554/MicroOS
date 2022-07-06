#include <syscall/syscalls.h>

#include <driver/char_input_driver.h>

cpu_registers_t* sys_async_getc(cpu_registers_t* regs) {
	regs->ebx = global_char_input_driver->async_getc(global_char_input_driver);

	return regs;
}