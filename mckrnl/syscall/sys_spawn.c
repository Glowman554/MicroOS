#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <scheduler/scheduler.h>
#include <stdio.h>
#include <string.h>

cpu_registers_t* sys_spawn(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	char** argv = (char**) regs->ecx;
	char** envp = (char**) regs->edx;

	if (path == NULL) {
		abortf(true, "sys_spawn: path is NULL");
	}

	file_t* file = vfs_open(path, FILE_OPEN_MODE_READ);
	if (!file) {
		debugf("Failed to open %s", path);
		regs->esi = -1;
		return regs;
	}

	task_t* current = get_self();

	void* buffer = vmm_alloc(file->size / 4096 + 1);
	vfs_read(file, buffer, file->size, 0);
	regs->esi = init_executable(current->term, buffer, argv, envp);
	vmm_free(buffer, file->size / 4096 + 1);
	vfs_close(file);

	task_t* spawned_task = get_task_by_pid(regs->esi);

	debugf("copying pwd: %s", current->pwd);
	size_t pwd_len = strlen(current->pwd);
	if (pwd_len >= sizeof(spawned_task->pwd)) {
		pwd_len = sizeof(spawned_task->pwd) - 1;
	}
	memcpy(spawned_task->pwd, current->pwd, pwd_len);
	spawned_task->pwd[pwd_len] = '\0';

	return regs;
}