#include <syscall/syscalls.h>

#include <driver/mouse_driver.h>
#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>
#include <string.h>

cpu_registers_t* sys_mouse_info(cpu_registers_t* regs) {
    task_t* current = get_self();

	if (global_char_output_driver->current_term == current->term) {
        *(mouse_info_t*) regs->ebx = global_mouse_driver->info;
	} else {
        memset((mouse_info_t*) regs->ebx, 0, sizeof(mouse_info_t));
	}

	return regs;
}