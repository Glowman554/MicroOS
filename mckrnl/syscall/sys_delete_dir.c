#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>

cpu_registers_t* sys_delete_dir(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	vfs_delete_dir(path);
	
	return regs;
}