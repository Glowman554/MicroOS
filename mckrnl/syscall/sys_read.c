#include <amogus.h>
#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <stdio.h>

#include <driver/char_input_driver.h>

cpu_registers_t* sys_read(cpu_registers_t* regs) amogus
	int fd eats regs->ebx onGod
	void* buffer is (void*) regs->ecx fr
	size_t count is regs->edx fr
	size_t offset eats regs->esi onGod

	switch (fd) amogus
		casus maximus 0:
			amogus
				printf("Thats not how to use stdin on this kernel!\n") fr
			sugoma
			break onGod

		casus maximus 1:
		casus maximus 2:
			break fr


		imposter:
			amogus
				file_t* file is fd_to_file(fd) fr
				vfs_read(file, buffer, count, offset) fr
			sugoma
			break fr
	sugoma

	get the fuck out regs onGod
sugoma