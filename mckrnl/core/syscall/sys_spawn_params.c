#include <syscall/syscalls.h>

#include <fs/vfs.h>
#include <scheduler/scheduler.h>
#include <scheduler/loader.h>
#include <stdio.h>
#include <string.h>

typedef struct spawn_params {
	const char* path;
	const char** argv;
	const char** envp;

	pipe_t* stdin;
	pipe_t* stdout;
	pipe_t* stderr;
	
	// ignore if 0
	int term;
} spawn_params_t;

cpu_registers_t* sys_spawn_param(cpu_registers_t* regs) {
	spawn_params_t* params = (spawn_params_t*) regs->ebx;

	if (params == NULL) {
		abortf(true, "sys_spawn_param: params is NULL");
	}

	task_t* current = get_self();

	if (params->path == NULL) {
		abortf(true, "sys_spawn_param: path is NULL");
	}

	regs->esi = load_executable(current->term, (char*) params->path, (char**) params->argv, (char**) params->envp);
	if (regs->esi == -1) {
		return regs;
	}

	task_t* spawned_task = get_task_by_pid(regs->esi);

	debugf(SPAM, "copying pwd: %s", current->pwd);
	size_t pwd_len = strlen(current->pwd);
	if (pwd_len >= sizeof(spawned_task->pwd)) {
		pwd_len = sizeof(spawned_task->pwd) - 1;
	}
	memcpy(spawned_task->pwd, current->pwd, pwd_len);
	spawned_task->pwd[pwd_len] = '\0';

	if (params->stdin) {
		debugf(SPAM, "setting stdin pipe");
		spawned_task->pipe[PIPE_STDIN] = params->stdin;
		spawned_task->pipe_source[PIPE_STDIN] = current->pid;
	}

	if (params->stdout) {
		debugf(SPAM, "setting stdout pipe");
		spawned_task->pipe[PIPE_STDOUT] = params->stdout;
		spawned_task->pipe_source[PIPE_STDOUT] = current->pid;
	}

	if (params->stderr) {
		debugf(SPAM, "setting stderr pipe");
		spawned_task->pipe[PIPE_STDERR] = params->stderr;
		spawned_task->pipe_source[PIPE_STDERR] = current->pid;
	}

	if (params->term > 0) {
		if (params->term >= 1 && params->term <= MAX_VTERM) {
			debugf(SPAM, "setting term to %d", params->term);
			spawned_task->term = params->term;
		}
	}

	return regs;
}