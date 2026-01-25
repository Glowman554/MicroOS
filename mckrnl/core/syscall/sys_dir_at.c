#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>

#include <string.h>

#include <stdio.h>

cpu_registers_t* sys_dir_at(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	int idx = regs->ecx;
	dir_t* user_dir = (dir_t*) regs->edx;

	if (user_dir == NULL || path == NULL) {
		abortf(true, "sys_dir_at: user_dir or path is NULL");
	}

	dir_t dir = vfs_dir_at(idx, path);

	memcpy(user_dir, &dir, sizeof(dir_t));
	
	return regs;
}