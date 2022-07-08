#include <commands.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/spawn.h>
#include <sys/env.h>
#include <sys/file.h>

#include <assert.h>

#include <argv_tools.h>

#define GET_CWD(cwd) char cwd[64] = { 0 }; set_env(SYS_GET_PWD_ID, cwd)

char* term_stdout = NULL;
int term_stdout_size = 1;

char* term_stdin = NULL;
int term_stdin_size = 1;
int stdin_pos = 0;

bool command_received(char* command, bool* should_break, char* stdin) {
	*should_break = false;

	int token_pos = 0;
	bool double_pipe_symbol = false;
	int cmd_type = get_command_type(command, &token_pos, &double_pipe_symbol);

	//printf("%d\n", cmd_type);

	char** command_stdin = NULL;
	if (stdin != NULL) {
		command_stdin = &stdin;
	}

	if (cmd_type == NORMAL) { //Normal command, just run it and send the stdin
		int found_command = run_command(command, terminal_envp, should_break, command_stdin, NULL);
		if (!found_command) {
			printf("Error: Command not found: '%s'\n", command);
		}
		return found_command;
	} else if (cmd_type == AND_RUN || cmd_type == PIPE_PROC || cmd_type == PIPE_FILE || cmd_type == PIPE_FILE_APPEND) {
		char* current_command = process_line(command, false); //This is the first command that needs to be run
		current_command[token_pos - (double_pipe_symbol ? 1 : 0)] = '\0';
		char* next_command = process_line(&current_command[token_pos + 1], false); //This could be a file name, or another command

		char* stdout = NULL; //Set to NULL by default, so that stdout isn't send if it isn't needed
		if (cmd_type == PIPE_PROC || cmd_type == PIPE_FILE || cmd_type == PIPE_FILE_APPEND) { //If the command is a pipe, we need to set the stdout to a buffer
			stdout = malloc(1);
			memset(stdout, 0, 1);
		}

		bool found_command = run_command(current_command, terminal_envp, should_break, command_stdin, &stdout); //Run the first command with the stdin, and stdout if there is one
		if (!found_command) {
			printf("Error: command not found: %s\n", current_command);
			return false;
		}

		bool call_next_command = true;
		if (cmd_type == PIPE_FILE || cmd_type == PIPE_FILE_APPEND) { //If the command is a pipe to a file, we need to open the file and write the stdout to it
			int old_cmd_type = cmd_type;
			current_command = next_command; //There may be another command after this one, so we will need to run it if so
			cmd_type = get_command_type(current_command, &token_pos, &double_pipe_symbol);
			if (cmd_type == NORMAL) {
				call_next_command = false;
			} else {
				current_command[token_pos - (double_pipe_symbol ? 1 : 0)] = '\0';
				call_next_command = true;
				next_command = process_line(&current_command[token_pos + 1], false);
			}

			char file_to_create[256];
			memset(file_to_create, 0, 256);
			bool canresolve = resolve_check(current_command, file_to_create, false);
			if (!canresolve) {
				printf("Error: No such file or directory to create output file in: %s\n", current_command);
				return false;
			}

			FILE* file = fopen(file_to_create, "w");
			if (file == NULL) {
				printf("Error: Failed to open file: %s\n", file_to_create);
				return false;
			}

			if (old_cmd_type == PIPE_FILE_APPEND) {
				fseek(file, 0, SEEK_END);
			}

			fwrite(stdout, strlen(stdout), 1, file);
			fclose(file);
		}

		if (call_next_command) { //If the command is a pipe to another command (or there is another command after the file), we need to run the next command with the stdout as the stdin
			found_command = command_received(next_command, should_break, stdout);
		}

		if (stdout != NULL) { //Make sure to free the stdout if it was allocated
			free(stdout);
		}

		return found_command;
	} else {
		printf("Error: Got an unknown command type, this shouldn't happen. %s\n", command);
	}

	return false;
}


void append_stdout(char* str, uint64_t size) {
	int old_size = term_stdout_size;
	term_stdout_size += size;

	term_stdout = realloc(term_stdout, term_stdout_size);
	memcpy(&term_stdout[old_size - 1], str, size);
	term_stdout[term_stdout_size - 1] = '\0';
}

void append_stdin(char* buffer, uint64_t size) {
	int can_copy = (term_stdin_size - 1) - stdin_pos;
	if (size > can_copy) {
		int left_over = size - can_copy;
		if (can_copy != 0) {
			memcpy(buffer, &term_stdin[stdin_pos], can_copy);
			stdin_pos += can_copy;
		}

		// env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0);

		// char input[left_over];
		// read(STDIN, input, left_over, 0);
		// memcpy(buffer + can_copy, input, left_over);

		// env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1);

		memset(buffer + can_copy, 0, left_over);
	} else {
		memcpy(buffer, &term_stdin[stdin_pos], size);
		stdin_pos += size;
	}
}

int term_printf(const char *fmt, ...) {
	char printf_buf[1024 * 4];
	va_list args;
	int printed;

	va_start(args, fmt);
	printed = vsprintf(printf_buf, fmt, args);
	va_end(args);

	if (term_stdout == NULL) {
		write(STDOUT, printf_buf, printed, 0);
	} else {
		append_stdout(printf_buf, printed);
	}

	return printed;
}

bool command_received(char* command, bool* should_break, char* stdin);
void system_command_handler(char* in) {
	bool should_break = false;
	command_received(in, &should_break, NULL);
}

bool run_command(char* command, char** terminal_envp, bool* should_break, char** stdin, char** stdout) {
	if (stdout != NULL) {
		term_stdout = *stdout;
		term_stdout_size = strlen(term_stdout) + 1;
	}

	if (stdin != NULL) {
		term_stdin = *stdin;
		term_stdin_size = strlen(term_stdin) + 1;
		stdin_pos = 0;
	}

	if (strncmp(command, (char*)"cd ", 3) == 0) {
		char** argv = argv_split(command);
		argv = argv_env_process(argv);

		cd(argv);

		free_argv(argv);
	} else if (strncmp(command, (char*)"pwd", 3) == 0) {
		pwd();
	} else if (strncmp(command, (char*)"export ", 7) == 0) {
		char* argv_str = read_env(command);
		export(argv_str);
		free(argv_str);
	} else if (strncmp(command, (char*)"read ", 5) == 0) {
		read_(command);
	} else if (strcmp(command, (char*)"exit") == 0) {
		*should_break = true;
	} else {
		char** argv = argv_split(command);
		argv = argv_env_process(argv);

		pipe stdout_pipe = NULL;
		if (term_stdout != NULL) {
			stdout_pipe = append_stdout;
		}

		pipe stdin_pipe = NULL;
		if (term_stdin != NULL) {
			stdin_pipe = append_stdin;
		}

		int pid = spawn_process(argv, terminal_envp, stdout_pipe, stdin_pipe);
		if (pid == -1) {
			return false;
		}

		free_argv(argv);
	}

	if (stdout != NULL) {
		*stdout = term_stdout;
	}
	term_stdout = NULL;
	term_stdout_size = 1;

	if (stdin != NULL) {
		*stdin = term_stdin;
	}
	term_stdin = NULL;
	term_stdin_size = 1;
	stdin_pos = 0;

	return true;
}

char* search_executable(char* command) {
	char* path = getenv("PATH");

	if (path == NULL) {
		exit(-1);
	}

	char* path_copy = malloc(strlen(path) + 1);
	memset(path_copy, 0, strlen(path) + 1);
	strcpy(path_copy, path);
	char* path_token = strtok(path_copy, ";");

	while (path_token != NULL) {
		char* executable = malloc(strlen(path_token) + strlen(command) + 2);
		memset(executable, 0, strlen(path_token) + strlen(command) + 2);
		strcpy(executable, path_token);
		strcat(executable, "/");
		strcat(executable, command);

		int fd;
		if ((fd = open(executable, FILE_OPEN_MODE_READ)) != -1) {
			close(fd);
			free(path_copy);
			return executable;
		}

		free(executable);

		char* executable2 = malloc(strlen(path_token) + strlen(command) + strlen(".elf") + 2);
		memset(executable2, 0, strlen(path_token) + strlen(command) + strlen(".elf") + 2);
		strcpy(executable2, path_token);
		strcat(executable2, "/");
		strcat(executable2, command);
		strcat(executable2, ".elf");

		if ((fd = open(executable2, FILE_OPEN_MODE_READ)) != -1) {
			close(fd);
			free(path_copy);
			return executable2;
		}

		free(executable2);
		path_token = strtok(NULL, ";");
	}

	free(path_copy);

	char* command_copy = malloc(strlen(command) + 1);
	memset(command_copy, 0, strlen(command) + 1);
	strcpy(command_copy, command);
	return command_copy;
}

void cd(char** argv) {
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}

	char path_buf[256];
	memset(path_buf, 0, 256);
	bool cancd = false;

	if (argc == 1) {
		char* env = getenv("ROOT_FS");
		if (env != NULL) {
			cancd = resolve(env, path_buf);
		} else {
			term_printf("No root filesystem specified!\n");
			return;
		}
	} else if (argc == 2) {
		cancd = resolve(argv[1], path_buf);
	} else {
		term_printf("Too many arguments.");
		return;
	}

	if (!cancd) {
		term_printf("No such file or directory: %s\n", path_buf);
		return;
	}

	int fd = open(path_buf, FILE_OPEN_MODE_READ);
	if (fd != -1) {
		term_printf("You can only change to a folder!\n");
		close(fd);
		return;
	}

	set_env(SYS_SET_PWD_ID, path_buf);
}

extern char** terminal_envp;

void export(char* command) {
	if (strlen(command) <= 7) {
		term_printf("No environment variable specified! Try like this: export MYVAR=value\n");
		return;
	}

	char* env_var = command + 7;

	char* env_name_tmp = malloc(strlen(env_var) + 1); //Make a copy of the string to operate on it
	memset(env_name_tmp, 0, strlen(env_var) + 1);
	strcpy(env_name_tmp, env_var);
	
	char* env_name = strtok(env_name_tmp, "=");
	if (strcmp(env_var, env_name) == 0) {
		term_printf("No environment variable value specified! Try like this: export MYVAR=value\n");
		free(env_name_tmp); //Make sure to free the memory allocated for strtok
		return;
	}
	size_t env_name_len = strlen(env_name);

	bool found = false;
	int next_empty_env_var = 0;
	for (int i = 0; terminal_envp[i] != NULL; i++) {
		if (strncmp(env_name, terminal_envp[i], env_name_len) == 0) {
			found = true;

			free(terminal_envp[i]); //Delete the old environment variable

			terminal_envp[i] = malloc(strlen(env_var) + 1); //Replace it with the new one
			memset(terminal_envp[i], 0, strlen(env_var) + 1);
			strcpy(terminal_envp[i], env_var);

			break;
		}
		next_empty_env_var = i + 1;
	}

	if (!found) { //We haven't found the environment variable, so we need to add it
		terminal_envp[next_empty_env_var] = malloc(strlen(env_var) + 1);
		memset(terminal_envp[next_empty_env_var], 0, strlen(env_var) + 1);
		strcpy(terminal_envp[next_empty_env_var], env_var);
		terminal_envp[next_empty_env_var + 1] = NULL;
	}

	free(env_name_tmp);
}

void read_(char* command) {
	if (strlen(command) <= 5) {
		term_printf("No env var specified! Try like this: read output_var_name\n");
		return;
	}

	char* env_var = command + 5;
	char in[512] = { 0 };
	int len = 0;
	bool reading = true;

	while (reading) {
		in[len] = getchar();
		putchar(in[len]);
		if(in[len] == '\n') {
			in[len] = '\0';
			reading = false;
		} if (in[len] == '\b') {
			in[len] = '\0';
			len--;
		} else {
			len++;
		}
	}

	char export_cmd[512] = { 0 };
	sprintf(export_cmd, "export %s=%s", env_var, in);
	export(export_cmd);
}

void pwd() {
	GET_CWD(cwd);
	term_printf("%s\n", cwd);
}

int spawn_process(char** argv, char** terminal_envp, pipe stdout, pipe stdin) {
	assert(stdout == NULL);
	assert(stdin == NULL);

	char* executable = search_executable((char*) argv[0]);
	const char** envp = (const char**) terminal_envp;

	int pid = spawn(executable, (const char**) argv, envp);

	if (pid == -1) {
		return -1;
	}

	while (get_proc_info(pid)) {
		yield();
	}

	free(executable);
	return pid;
}