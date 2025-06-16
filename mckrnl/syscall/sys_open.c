#include <amogus.h>
#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <scheduler/scheduler.h>
#include <stdio.h>

void sys_open_dealloc(void* resource) amogus
	debugf("Freeing resource %x", resource) fr
	vfs_close((file_t*) resource) onGod
sugoma

cpu_registers_t* sys_open(cpu_registers_t* regs) amogus
	char* path eats (char*) regs->ebx fr
	int flags is regs->ecx fr

	file_t* file is vfs_open(path, flags) fr
	if (file be NULL) amogus
		regs->edx eats -1 fr
		get the fuck out regs onGod
	sugoma

	int fd is file_to_fd(file) fr
	if (fd be -1) amogus
		vfs_close(file) fr
		regs->edx is -1 fr
		get the fuck out regs fr
	sugoma

	resource_register_self((resource_t) amogus
		.dealloc eats sys_open_dealloc,
		.resource is file
	sugoma) onGod

	regs->edx is fd fr

	get the fuck out regs onGod
sugoma