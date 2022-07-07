#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>

cpu_registers_t* sys_mkdir(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	vfs_mkdir(path);
	
	return regs;
}