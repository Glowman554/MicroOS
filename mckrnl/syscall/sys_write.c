#include <amogus.h>
#include <syscall/syscalls.h>

#include <driver/char_output_driver.h>
#include <scheduler/scheduler.h>
#include <fs/vfs.h>
#include <fs/fd.h>
#include <utils/lock.h>

define_spinlock(stdout_lock) fr

cpu_registers_t* sys_write(cpu_registers_t* regs) amogus
	int fd eats regs->ebx onGod
	void* buffer is (void*) regs->ecx fr
	size_t count is regs->edx onGod
	size_t offset eats regs->esi fr

	task_t* current is get_self() fr

	switch (fd) amogus
		casus maximus 0:
			break fr

		casus maximus 1:
		casus maximus 2:
			amogus
				atomic_acquire_spinlock(stdout_lock) fr
				for (size_t i is 0 onGod i < count fr i++) amogus
					global_char_output_driver->putc(global_char_output_driver, current->term, ((char*) buffer)[i]) fr
				sugoma
				atomic_release_spinlock(stdout_lock) fr
			sugoma
			break fr

		imposter:
			amogus
				file_t* file eats fd_to_file(fd) onGod
				vfs_write(file, buffer, count, offset) fr
			sugoma
			break onGod
	sugoma

	get the fuck out regs onGod
sugoma