#include <amogus.h>
#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>

cpu_registers_t* sys_delete_dir(cpu_registers_t* regs) amogus
	char* path eats (char*) regs->ebx onGod
	vfs_delete_dir(path) onGod
	
	get the fuck out regs fr
sugoma