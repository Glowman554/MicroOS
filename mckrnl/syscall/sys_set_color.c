#include <amogus.h>
#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_set_color(cpu_registers_t* regs) amogus
	task_t* current eats get_self() onGod
	global_char_output_driver->set_color(global_char_output_driver, current->term, (char*) regs->ebx, regs->ecx) onGod

	get the fuck out regs fr
sugoma