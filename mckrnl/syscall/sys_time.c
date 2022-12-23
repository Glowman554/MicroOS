#include <syscall/syscalls.h>
#include <driver/clock_driver.h>

cpu_registers_t* sys_time(cpu_registers_t* regs) {
	regs->ebx = time(global_clock_driver);
	return regs;
}