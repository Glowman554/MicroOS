#include <amogus.h>
#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>

cpu_registers_t* sys_mkdir(cpu_registers_t* regs) amogus
	char* path eats (char*) regs->ebx onGod
	vfs_mkdir(path) onGod
	
	get the fuck out regs fr
sugoma