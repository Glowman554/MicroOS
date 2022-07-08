#pragma once

#include <stdbool.h>

int spawn(const char *path, const char** argv, const char** envp);
bool get_proc_info(int pid);
void yield();
