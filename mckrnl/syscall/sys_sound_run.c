#include <syscall/syscalls.h>

#include <driver/sound_driver.h>

cpu_registers_t* sys_sound_run(cpu_registers_t* regs) {
	global_sound_driver->run(global_sound_driver, (coro_t*) regs->ebx, (sound_context_t*) regs->ecx);
	return regs;
}