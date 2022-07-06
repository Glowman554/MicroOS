#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>

cpu_registers_t* sys_close(cpu_registers_t* regs) {
	int fd = regs->ebx;

	file_t* file = fd_to_file(fd);
	vfs_close(file);

	return regs;
}