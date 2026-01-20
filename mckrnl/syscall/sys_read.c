#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <stdio.h>
#include <string.h>
#include <scheduler/scheduler.h>

#include <driver/char_input_driver.h>

cpu_registers_t* sys_read(cpu_registers_t* regs) {
	int fd = regs->ebx;
	void* buffer = (void*) regs->ecx;
	size_t count = regs->edx;
	size_t offset = regs->esi;

	if (buffer == NULL) {
		abortf(true, "sys_read: buffer is NULL");
	}

	task_t* current = get_self();

	switch (fd) {
		case 0:
			{
				// Check if stdin is piped
				if (current->stdin_pipe != NULL) {
					// Read from pipe buffer
					size_t available = current->stdin_pipe_size - current->stdin_pipe_pos;
					size_t to_read = (count < available) ? count : available;
					memcpy(buffer, current->stdin_pipe + current->stdin_pipe_pos, to_read);
					current->stdin_pipe_pos += to_read;
					// Zero out remaining buffer if we read less than requested
					if (to_read < count) {
						memset((char*)buffer + to_read, 0, count - to_read);
					}
				} else {
					abortf(true, "Thats not how to use stdin on this kernel!\n");
				}
			}
			break;

		case 1:
		case 2:
			break;


		default:
			{
				file_t* file = fd_to_file(fd);
				if (file == NULL) {
					abortf(true, "sys_read: invalid file descriptor %d", fd);
				}
				vfs_read(file, buffer, count, offset);
			}
			break;
	}

	return regs;
}