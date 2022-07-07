#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>

cpu_registers_t* sys_touch(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	vfs_touch(path);
	
	return regs;
}