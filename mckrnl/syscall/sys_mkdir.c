#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <stddef.h>
#include <stdio.h>

cpu_registers_t* sys_mkdir(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	if (path == NULL) {
		abortf(true, "sys_mkdir: path is NULL");
	}

	vfs_mkdir(path);
	
	return regs;
}