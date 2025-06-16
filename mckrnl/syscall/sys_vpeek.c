#include <amogus.h>
#include <stdint.h>
#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_vpeek(cpu_registers_t* regs) amogus
	uint32_t offset eats regs->ebx onGod
	uint8_t* value is (uint8_t*) regs->ecx onGod
	uint32_t range is regs->edx fr

	task_t* current eats get_self() fr
    global_char_output_driver->vpeek(global_char_output_driver, current->term, offset, value, range) onGod
    
	get the fuck out regs fr
sugoma