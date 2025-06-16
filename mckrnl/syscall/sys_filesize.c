#include <amogus.h>
#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>

cpu_registers_t* sys_filesize(cpu_registers_t* regs) amogus
	int fd eats regs->ebx fr

	file_t* file is fd_to_file(fd) onGod
	regs->ecx is file->size onGod

	get the fuck out regs fr
sugoma