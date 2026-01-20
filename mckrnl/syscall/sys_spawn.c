#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <scheduler/scheduler.h>
#include <scheduler/loader.h>
#include <stdio.h>
#include <string.h>
#include <memory/vmm.h>

cpu_registers_t* sys_spawn(cpu_registers_t* regs) {
	char* path = (char*) regs->ebx;
	char** argv = (char**) regs->ecx;
	char** envp = (char**) regs->edx;
	bool enable_stdout_pipe = (bool) regs->esi;
	bool enable_stdin_pipe = (bool) regs->edi;

	if (path == NULL) {
		abortf(true, "sys_spawn: path is NULL");
	}

	task_t* current = get_self();

	regs->esi = load_executable(current->term, path, argv, envp);
	if (regs->esi == -1) {
		return regs;
	}

	task_t* spawned_task = get_task_by_pid(regs->esi);

	debugf("copying pwd: %s", current->pwd);
	size_t pwd_len = strlen(current->pwd);
	if (pwd_len >= sizeof(spawned_task->pwd)) {
		pwd_len = sizeof(spawned_task->pwd) - 1;
	}
	memcpy(spawned_task->pwd, current->pwd, pwd_len);
	spawned_task->pwd[pwd_len] = '\0';

	// Initialize pipe buffers if requested (1 page = 4096 bytes)
	if (enable_stdout_pipe) {
		spawned_task->stdout_pipe = (char*) vmm_calloc(1);
		spawned_task->stdout_pipe_size = 0;
		spawned_task->stdout_pipe_capacity = 4096;
	} else {
		spawned_task->stdout_pipe = NULL;
		spawned_task->stdout_pipe_size = 0;
		spawned_task->stdout_pipe_capacity = 0;
	}

	if (enable_stdin_pipe) {
		spawned_task->stdin_pipe = (char*) vmm_calloc(1);
		spawned_task->stdin_pipe_size = 0;
		spawned_task->stdin_pipe_pos = 0;
	} else {
		spawned_task->stdin_pipe = NULL;
		spawned_task->stdin_pipe_size = 0;
		spawned_task->stdin_pipe_pos = 0;
	}

	return regs;
}