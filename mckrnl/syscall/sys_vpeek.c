#include <stdint.h>
#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>

cpu_registers_t* sys_vpeek(cpu_registers_t* regs) {
	uint32_t offset = regs->ebx;
	uint8_t* value = (uint8_t*) regs->ecx;
	uint32_t range = regs->edx;

    global_char_output_driver->vpeek(global_char_output_driver, offset, value, range);
    
	return regs;
}