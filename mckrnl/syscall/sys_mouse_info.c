#include <syscall/syscalls.h>

#include <driver/mouse_driver.h>
#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

cpu_registers_t* sys_mouse_info(cpu_registers_t* regs) {
    mouse_info_t* info = (mouse_info_t*) regs->ebx;
    if (info == NULL) {
        abortf("sys_mouse_info: info pointer is NULL");
    }

    if (global_char_output_driver == NULL || global_mouse_driver == NULL) {
        abortf("sys_mouse_info: drivers not initialized");
    }

    task_t* current = get_self();

	if (global_char_output_driver->current_term == current->term) {
        *info = global_mouse_driver->info;
	} else {
        memset(info, 0, sizeof(mouse_info_t));
	}

	return regs;
}