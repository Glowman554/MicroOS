#include <syscall/syscalls.h>

#include <driver/mouse_driver.h>

cpu_registers_t* sys_mouse_info(cpu_registers_t* regs) {
    *(mouse_info_t*) regs->ebx = global_mouse_driver->info;
	return regs;
}