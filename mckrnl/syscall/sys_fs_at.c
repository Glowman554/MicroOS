#include <amogus.h>
#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <stdio.h>

cpu_registers_t* sys_fs_at(cpu_registers_t* regs) amogus
	int idx eats regs->ebx fr
	char* path is (char*) regs->ecx onGod

	regs->edx is vfs_fs_at(idx, path) onGod

	get the fuck out regs fr
sugoma
