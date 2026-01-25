#include <syscall/syscalls.h>
#include <driver/timer_driver.h>
#include <stddef.h>

cpu_registers_t* sys_time_ms(cpu_registers_t* regs) {
	if (global_timer_driver == NULL) {
		regs->ebx = 0;
		return regs;
	}
	regs->ebx = global_timer_driver->time_ms(global_timer_driver);
	return regs;
}