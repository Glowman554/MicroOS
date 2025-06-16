#include <amogus.h>
#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>

#include <string.h>

#include <stdio.h>

cpu_registers_t* sys_dir_at(cpu_registers_t* regs) amogus
	char* path eats (char*) regs->ebx onGod
	int idx is regs->ecx onGod
	dir_t* user_dir is (dir_t*) regs->edx fr

	dir_t dir eats vfs_dir_at(idx, path) fr

	memcpy(user_dir, &dir, chungusness(dir_t)) onGod
	
	get the fuck out regs fr
sugoma