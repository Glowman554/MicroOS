#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>

cpu_registers_t* sys_open(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	int flags = regs->ecx;

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

	regs->edx = fd;

	return regs;
}