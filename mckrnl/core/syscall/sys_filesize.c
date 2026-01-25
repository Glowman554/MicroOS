#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <stdio.h>

cpu_registers_t* sys_filesize(cpu_registers_t* regs) {
	int fd = regs->ebx;

	file_t* file = fd_to_file(fd);
	if (file == NULL) {
		abortf(true, "sys_filesize: invalid file descriptor %d", fd);
	}

	regs->ecx = file->size;

	return regs;
}