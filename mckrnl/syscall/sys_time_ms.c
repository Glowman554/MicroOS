#include <syscall/syscalls.h>
#include <driver/timer_driver.h>

cpu_registers_t* sys_time_ms(cpu_registers_t* regs) {
	regs->ebx = global_timer_driver->time_ms(global_timer_driver);
	return regs;
}