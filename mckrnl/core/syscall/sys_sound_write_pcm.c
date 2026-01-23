#include <syscall/syscalls.h>

#include <driver/sound_driver.h>
#include <stddef.h>
#include <stdio.h>

cpu_registers_t* sys_sound_write_pcm(cpu_registers_t* regs) {
	uint8_t* buffer = (uint8_t*) regs->ebx;
	uint32_t size = regs->ecx;
	
	if (global_sound_driver == NULL || global_sound_driver->write_pcm == NULL) {
		abortf(true, "sys_sound_write_pcm");
	}
	
	if (buffer == NULL || size == 0) {
		abortf(true, "sys_sound_write_pcm: Invalid buffer");
	}
	
	uint64_t written = global_sound_driver->write_pcm(global_sound_driver, buffer, size);
	regs->eax = (uint32_t) written;
	return regs;
}
