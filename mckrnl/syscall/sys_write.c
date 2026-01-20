#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>
#include <fs/vfs.h>
#include <fs/fd.h>
#include <utils/lock.h>
#include <stdio.h>
#include <string.h>
#include <memory/vmm.h>

define_spinlock(stdout_lock);

cpu_registers_t* sys_write(cpu_registers_t* regs) {
	int fd = regs->ebx;
	void* buffer = (void*) regs->ecx;
	size_t count = regs->edx;
	size_t offset = regs->esi;

	if (buffer == NULL) {
		abortf(true, "sys_write: buffer is NULL");
		return regs;
	}

	task_t* current = get_self();

	switch (fd) {
		case 0:
			break;

		case 1:
		case 2:
			{
				// Check if stdout is piped
				if (current->stdout_pipe != NULL) {
					// Write to pipe buffer
					size_t new_size = current->stdout_pipe_size + count;
					if (new_size + 1 > current->stdout_pipe_capacity) {
						// Expand buffer (need space for null terminator)
						size_t old_pages = TO_PAGES(current->stdout_pipe_capacity);
						size_t new_capacity = current->stdout_pipe_capacity * 2;
						while (new_capacity <= new_size + 1) {
							new_capacity *= 2;
						}
						size_t new_pages = TO_PAGES(new_capacity);
						char* new_buffer = (char*) vmm_alloc(new_pages);
						memcpy(new_buffer, current->stdout_pipe, current->stdout_pipe_size);
						vmm_free(current->stdout_pipe, old_pages);
						current->stdout_pipe = new_buffer;
						current->stdout_pipe_capacity = new_capacity;
					}
					memcpy(current->stdout_pipe + current->stdout_pipe_size, buffer, count);
					current->stdout_pipe_size = new_size;
					current->stdout_pipe[current->stdout_pipe_size] = '\0';
				} else {
					// Normal stdout output
					atomic_acquire_spinlock(stdout_lock);
					for (size_t i = 0; i < count; i++) {
						global_char_output_driver->putc(global_char_output_driver, current->term, ((char*) buffer)[i]);
					}
					atomic_release_spinlock(stdout_lock);
				}
			}
			break;

		default:
			{
				file_t* file = fd_to_file(fd);
				if (file == NULL) {
					abortf(true, "sys_write: invalid file descriptor %d", fd);
					break;
				}
				vfs_write(file, buffer, count, offset);
			}
			break;
	}

	return regs;
}