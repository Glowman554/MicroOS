#include <syscall/syscalls.h>

#include <driver/sound_driver.h>
#include <stddef.h>
#include <stdio.h>

cpu_registers_t* sys_sound_get_sample_rate(cpu_registers_t* regs) {
	if (global_sound_driver == NULL || global_sound_driver->get_sample_rate == NULL) {
		abortf(true, "sys_sound_get_sample_rate");
	}
	
	regs->eax = global_sound_driver->get_sample_rate(global_sound_driver);
	return regs;
}
