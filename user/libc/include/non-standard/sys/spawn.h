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

int get_task_list(task_list_t* out, int max);

void kill(int pid);

int thread(void* entry);

void set_term(int pid, int term);
void set_pipe(int pid, pipe_t* pipe, int output);