#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <stddef.h>
#include <stdio.h>

cpu_registers_t* sys_touch(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	if (path == NULL) {
		abortf(true, "sys_touch: path is NULL");
	}
	vfs_touch(path);
	
	return regs;
}