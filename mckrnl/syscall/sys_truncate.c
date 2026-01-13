#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <scheduler/scheduler.h>
#include <stdio.h>

cpu_registers_t* sys_truncate(cpu_registers_t* regs) {
	int fd = regs->ebx;
	int new_size = regs->ecx;

	file_t* file = fd_to_file(fd);
	if (file == NULL) {
		abortf("sys_truncate: invalid file descriptor %d", fd);
	}
	vfs_truncate(file, new_size);

	return regs;
}