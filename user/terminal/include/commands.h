#pragma once

#include <stdbool.h>
#include <sys/spawn.h>

#define PIPE_BUFFER_SIZE 65536

extern char** terminal_envp;

bool command_received(char* command, bool* should_break, pipe_t* stdin_pipe, pipe_t* stdout_final_pipe);

bool run_command(char* command, char** terminal_envp, bool* should_break, pipe_t* stdin_pipe, pipe_t** stdout_pipe);

void set_layout(char* command, pipe_t* out);
void cd(char** argv, pipe_t* out);
void pwd(pipe_t* out);
void export(char* command, pipe_t* out);
void read_(char* command, pipe_t* out);
void fault(char* command, pipe_t* out);

pipe_t* make_pipe(char* buffer, int size, int pos);

int spawn_process(char** argv, char** terminal_envp, pipe_t* stdout, pipe_t* stdin);
