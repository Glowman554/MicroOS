#include <syscall/syscalls.h>

#include <driver/sound_driver.h>
#include <stddef.h>
#include <stdio.h>

cpu_registers_t* sys_sound_run(cpu_registers_t* regs) {
	if (global_sound_driver == NULL) {
		abortf("sys_sound_run: sound driver not initialized");
	}
	coro_t* coro = (coro_t*) regs->ebx;
	sound_context_t* ctx = (sound_context_t*) regs->ecx;
	if (coro == NULL || ctx == NULL) {
		abortf("sys_sound_run: coro or ctx is NULL");
	}
	global_sound_driver->run(global_sound_driver, coro, ctx);
	return regs;
}