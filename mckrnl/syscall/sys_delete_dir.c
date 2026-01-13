#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <stddef.h>
#include <stdio.h>

cpu_registers_t* sys_delete_dir(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	if (path == NULL) {
		abortf("sys_delete_dir: path is NULL");
	}
	vfs_delete_dir(path);
	
	return regs;
}