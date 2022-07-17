#pragma once

#include <stdbool.h>
#include <sys/spawn.h>

typedef void* pipe;

extern char** terminal_envp;

bool command_received(char* command, bool* should_break, char* stdin);

bool run_command(char* command, char** terminal_envp, bool* should_break, char** stdin, char** stdout);

char* search_executable(char* command);

void cd(char** argv);
void pwd();
void export(char* command);
void read_(char* command);
void fault(char* command);

int spawn_process(char** argv, char** terminal_envp, pipe stdout, pipe stdin);