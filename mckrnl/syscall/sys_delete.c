#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <scheduler/scheduler.h>
#include <stdio.h>

cpu_registers_t* sys_delete(cpu_registers_t* regs) {
	int fd = regs->ebx;

	file_t* file = fd_to_file(fd);
	if (!file) {
		abortf("sys_delete: invalid file descriptor %d", fd);
	}
	
	vfs_delete(file);
	resource_unregister_self(file);

	return regs;
}