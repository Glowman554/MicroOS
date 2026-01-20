#pragma once

#include <stdbool.h>
#include <stddef.h>

int spawn(const char *path, const char** argv, const char** envp);
int spawn_with_pipes(const char *path, const char** argv, const char** envp, bool enable_stdout_pipe, bool enable_stdin_pipe);

size_t pipe_read_stdout(int pid, void* buffer, size_t max_size);
void pipe_write_stdin(int pid, const void* buffer, size_t size);

bool get_proc_info(int pid);
void yield();

typedef struct task_list {
	char name[128];
	int pid;
	int term;
	int core;
} task_list_t;

int get_task_list(task_list_t* out, int max);

void kill(int pid);

int thread(void* entry);

void set_term(int pid, int term);