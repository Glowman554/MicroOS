#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_set_color(cpu_registers_t* regs) {
	task_t* current = get_self();
	global_char_output_driver->set_color(global_char_output_driver, current->term, (char*) regs->ebx, regs->ecx);

	return regs;
}