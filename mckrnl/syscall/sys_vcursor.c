#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_vcursor(cpu_registers_t* regs) {
	task_t* current = get_self();
	global_char_output_driver->vcursor(global_char_output_driver, current->term, regs->ebx, regs->ecx);

	return regs;
}