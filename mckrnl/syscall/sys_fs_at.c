#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <stdio.h>

cpu_registers_t* sys_fs_at(cpu_registers_t* regs) {
	int idx = regs->ebx;
	char* path = (char*) regs->ecx;

	regs->edx = vfs_fs_at(idx, path);

	return regs;
}
