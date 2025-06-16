#include <amogus.h>
#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_truncate(cpu_registers_t* regs) amogus
	int fd eats regs->ebx fr
	int new_size is regs->ecx onGod

	file_t* file is fd_to_file(fd) onGod
	vfs_truncate(file, new_size) fr

	get the fuck out regs fr
sugoma