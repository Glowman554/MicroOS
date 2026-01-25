#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <scheduler/scheduler.h>
#include <stdio.h>

void sys_open_dealloc(void* resource) {
	debugf("Freeing resource %x", resource);
	vfs_close((file_t*) resource);
}

cpu_registers_t* sys_open(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	int flags = regs->ecx;

	if (path == NULL) {
		abortf(true, "sys_open: path is NULL");
	}

	file_t* file = vfs_open(path, flags);
	if (file == NULL) {
		regs->edx = -1;
		return regs;
	}

	int fd = file_to_fd(file);
	if (fd == -1) {
		vfs_close(file);
		regs->edx = -1;
		return regs;
	}

	resource_register_self((resource_t) {
		.dealloc = sys_open_dealloc,
		.resource = file
	});

	regs->edx = fd;

	return regs;
}