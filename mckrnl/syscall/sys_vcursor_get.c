#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_vcursor_get(cpu_registers_t* regs) {
	task_t* current = get_self();
	global_char_output_driver->vcursor_get(global_char_output_driver, current->term, (int*) regs->ebx, (int*) regs->ecx);

	return regs;
}