#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>
#include <fs/vfs.h>
#include <fs/fd.h>

cpu_registers_t* sys_write(cpu_registers_t* regs) {
	int fd = regs->ebx;
	void* buffer = (void*) regs->ecx;
	size_t count = regs->edx;
	size_t offset = regs->esi;

	task_t* current = get_self();

	switch (fd) {
		case 0:
			break;

		case 1:
		case 2:
			{
				for (size_t i = 0; i < count; i++) {
					global_char_output_driver->putc(global_char_output_driver, current->term, ((char*) buffer)[i]);
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