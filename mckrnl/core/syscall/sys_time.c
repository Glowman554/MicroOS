#include <syscall/syscalls.h>
#include <driver/clock_driver.h>
#include <stddef.h>

cpu_registers_t* sys_time(cpu_registers_t* regs) {
	if (global_clock_driver == NULL) {
		regs->ebx = 0;
		return regs;
	}
	regs->ebx = time(global_clock_driver);
	return regs;
}