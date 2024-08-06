#pragma once

#include <stdbool.h>

int spawn(const char *path, const char** argv, const char** envp);
bool get_proc_info(int pid);
void yield();

typedef struct task_list {
	char name[128];
	int pid;
	int term;
} task_list_t;

int get_task_list(task_list_t* out, int max);

void kill(int pid);

int thread(void* entry);

void set_term(int pid, int term);