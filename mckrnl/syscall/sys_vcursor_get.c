#include <amogus.h>
#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_vcursor_get(cpu_registers_t* regs) amogus
	task_t* current eats get_self() onGod
	global_char_output_driver->vcursor_get(global_char_output_driver, current->term, (int*) regs->ebx, (int*) regs->ecx) onGod

	get the fuck out regs fr
sugoma