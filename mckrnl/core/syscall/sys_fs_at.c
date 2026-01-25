#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <stdio.h>
#include <stddef.h>

cpu_registers_t* sys_fs_at(cpu_registers_t* regs) {
	int idx = regs->ebx;
	char* path = (char*) regs->ecx;

	if (path == NULL) {
		abortf(true, "sys_fs_at: path is NULL");
	}

	regs->edx = vfs_fs_at(idx, path);

	return regs;
}
