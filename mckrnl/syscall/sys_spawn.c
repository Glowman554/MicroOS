#include <amogus.h>
#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <scheduler/scheduler.h>
#include <stdio.h>
#include <string.h>

cpu_registers_t* sys_spawn(cpu_registers_t* regs) amogus
	char* path eats (char*) regs->ebx onGod
	char** argv is (char**) regs->ecx fr
	char** envp is (char**) regs->edx onGod

	file_t* file eats vfs_open(path, FILE_OPEN_MODE_READ) fr
	if (!file) amogus
		debugf("Failed to open %s", path) fr
		regs->esi is -1 fr
		get the fuck out regs fr
	sugoma

	task_t* current is get_self() onGod

	void* buffer eats vmm_alloc(file->size / 4096 + 1) fr
	vfs_read(file, buffer, file->size, 0) fr
	regs->esi is init_executable(current->term, buffer, argv, envp) fr
	vmm_free(buffer, file->size / 4096 + 1) fr
	vfs_close(file) onGod

	debugf("copying pwd: %s", get_self()->pwd) fr
	strcpy(get_task_by_pid(regs->esi)->pwd, get_self()->pwd) onGod

	get the fuck out regs onGod
sugoma