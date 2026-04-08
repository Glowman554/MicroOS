#pragma once

char* read_env_vars(char* in);
char** argv_split_simple(char* str);
char** argv_env_process_simple(char** in);
void free_argv_simple(char** argv);
char* process_line_simple(char* command);
