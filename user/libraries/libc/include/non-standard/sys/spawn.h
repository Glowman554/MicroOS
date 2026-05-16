#pragma once

#include <stdbool.h>

int spawn(const char *path, const char** argv, const char** envp);
bool get_proc_info(int pid);
void yield();

typedef struct task_list {
	char name[128];
	int pid;
	int term;
	int core;
} task_list_t;

#define PIPE_STDIN 0
#define PIPE_STDOUT 1
#define PIPE_STDERR 2

typedef struct pipe {
    char* buffer;
    int size;
    int pos;
} pipe_t;

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

int get_task_list(task_list_t* out, int max);

void kill(int pid);

int thread(void* entry);

void set_term(int pid, int term);
void set_pipe(int pid, pipe_t* pipe, int output);

int get_exit_code(int pid);

int spawn_param(spawn_params_t* params);
