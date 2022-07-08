#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <scheduler/scheduler.h>
#include <stdio.h>

cpu_registers_t* sys_spawn(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	char** argv = (char**) regs->ecx;
	char** envp = (char**) regs->edx;

	file_t* file = vfs_open(path, FILE_OPEN_MODE_READ);
	if (!file) {
		debugf("Failed to open initrd:/bin/terminal.elf");
		regs->esi = -1;
		return regs;
	}

	void* buffer = vmm_alloc(file->size / 4096 + 1);
	vfs_read(file, buffer, file->size, 0);
	regs->esi = init_elf(buffer, argv, envp);
	vmm_free(buffer, file->size / 4096 + 1);
	vfs_close(file);

	return regs;
}