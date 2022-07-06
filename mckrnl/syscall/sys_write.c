#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <stdio.h>

cpu_registers_t* sys_write(cpu_registers_t* regs) {
	int fd = regs->ebx;
	void* buffer = (void*) regs->ecx;
	size_t count = regs->edx;
	size_t offset = regs->esi;

	switch (fd) {
		case 0:
			break;

		case 1:
		case 2:
			{
				for (size_t i = 0; i < count; i++) {
					printf("%c", ((char*) buffer)[i]);
				}
			}
			break;

		default:
			{
				file_t* file = fd_to_file(fd);
				vfs_write(file, buffer, count, offset);
			}
			break;
	}

	return regs;
}