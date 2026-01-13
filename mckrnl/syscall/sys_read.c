#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <stdio.h>

#include <driver/char_input_driver.h>

cpu_registers_t* sys_read(cpu_registers_t* regs) {
	int fd = regs->ebx;
	void* buffer = (void*) regs->ecx;
	size_t count = regs->edx;
	size_t offset = regs->esi;

	if (buffer == NULL) {
		abortf("sys_read: buffer is NULL");
	}

	switch (fd) {
		case 0:
			{
				printf("Thats not how to use stdin on this kernel!\n");
			}
			break;

		case 1:
		case 2:
			break;


		default:
			{
				file_t* file = fd_to_file(fd);
				if (file == NULL) {
					abortf("sys_read: invalid file descriptor %d", fd);
				}
				vfs_read(file, buffer, count, offset);
			}
			break;
	}

	return regs;
}