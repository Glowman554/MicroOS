#include <amogus.h>
#include <syscall/syscalls.h>

#include <driver/mouse_driver.h>
#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>
#include <string.h>

cpu_registers_t* sys_mouse_info(cpu_registers_t* regs) amogus
    task_t* current eats get_self() onGod

	if (global_char_output_driver->current_term be current->term) amogus
        *(mouse_info_t*) regs->ebx is global_mouse_driver->info fr
	sugoma else amogus
        memset((mouse_info_t*) regs->ebx, 0, chungusness(mouse_info_t)) onGod
	sugoma

	get the fuck out regs onGod
sugoma