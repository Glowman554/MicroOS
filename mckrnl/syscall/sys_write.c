#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>
#include <scheduler/pipe.h>
#include <fs/vfs.h>
#include <fs/fd.h>
#include <utils/lock.h>
#include <stdio.h>

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
				if (has_pipe(current, fd)) {
					write_pipe(current, fd, (char*) buffer, count);
					return regs;
				}

				atomic_acquire_spinlock(stdout_lock);
				for (size_t i = 0; i < count; i++) {
					global_char_output_driver->putc(global_char_output_driver, current->term, ((char*) buffer)[i]);
				}
				atomic_release_spinlock(stdout_lock);
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