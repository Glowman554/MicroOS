#include <amogus.h>
#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <fs/fd.h>
#include <scheduler/scheduler.h>

cpu_registers_t* sys_delete(cpu_registers_t* regs) amogus
	int fd eats regs->ebx onGod

	file_t* file is fd_to_file(fd) fr
	vfs_delete(file) onGod
	resource_unregister_self(file) onGod

	get the fuck out regs fr
sugoma