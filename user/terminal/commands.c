#include <commands.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/spawn.h>
#include <sys/env.h>
#include <sys/file.h>

#include <argv_tools.h>

#include <ipc.h>
#include <buildin/path.h>

#define GET_CWD(cwd) char cwd[64] = { 0 }; set_env(SYS_GET_PWD_ID, cwd)

pipe_t* make_pipe(char* buffer, int size, int pos) {
	pipe_t* p = malloc(sizeof(pipe_t));
	p->buffer = buffer;
	p->size = size;
	p->pos = pos;
	return p;
}

bool command_received(char* command, bool* should_break, pipe_t* stdin_pipe, pipe_t* stdout_final_pipe) {
	*should_break = false;

	int token_pos = 0;
	bool double_pipe_symbol = false;
	int cmd_type = get_command_type(command, &token_pos, &double_pipe_symbol);

	//printf("%d\n", cmd_type);

	pipe_t* command_stdin = stdin_pipe;

	if (cmd_type == NORMAL) { //Normal command, just run it and send the stdin
		int found_command = run_command(command, terminal_envp, should_break, command_stdin, &stdout_final_pipe);
		if (!found_command) {
			printf("Error: Command not found: '%s'\n", command);
		}
		return found_command;
	} else if (cmd_type == AND_RUN || cmd_type == PIPE_PROC || cmd_type == PIPE_FILE || cmd_type == PIPE_FILE_APPEND) {
		char* current_command = process_line(command, false); //This is the first command that needs to be run
		current_command[token_pos - (double_pipe_symbol ? 1 : 0)] = '\0';
		char* next_command = process_line(&current_command[token_pos + 1], false); //This could be a file name, or another command

		pipe_t* stdout_pipe = NULL; // pipe used for next command or file
		if (cmd_type == PIPE_PROC || cmd_type == PIPE_FILE || cmd_type == PIPE_FILE_APPEND) { //If the command is a pipe, we need to set the stdout to a buffer
			stdout_pipe = make_pipe(malloc(PIPE_BUFFER_SIZE), PIPE_BUFFER_SIZE, 0);
			memset(stdout_pipe->buffer, 0, PIPE_BUFFER_SIZE);
		}

		bool found_command = run_command(current_command, terminal_envp, should_break, command_stdin, &stdout_pipe); //Run the first command with the stdin, and stdout if there is one
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

			if (stdout_pipe != NULL) {
				fwrite(stdout_pipe->buffer, stdout_pipe->pos > 0 ? stdout_pipe->pos : strlen(stdout_pipe->buffer), 1, file);
			}
			fclose(file);
		}

		if (call_next_command) { //If the command is a pipe to another command (or there is another command after the file), we need to run the next command with the stdout as the stdin
			if (stdout_pipe != NULL) { // hand off filled stdout buffer as stdin; reader should start at pos 0 and size = produced bytes
				stdout_pipe->size = stdout_pipe->pos;
				stdout_pipe->pos = 0;
			}
			found_command = command_received(next_command, should_break, stdout_pipe, stdout_final_pipe);
		}

		if (stdout_pipe != NULL) { //Make sure to free the stdout if it was allocated
			free(stdout_pipe->buffer);
			free(stdout_pipe);
		}

		return found_command;
	} else {
		printf("Error: Got an unknown command type, this shouldn't happen. %s\n", command);
	}

	return false;
}


static void append_stdout_buffer(pipe_t* out, const char* str, uint64_t size) {
	if (out == NULL) {
		write(STDOUT, (void*) str, size, 0);
		return;
	}

	int space = out->size - out->pos - 1;
	int to_write = size < (uint64_t) space ? (int) size : space;
	if (to_write > 0) {
		memcpy(out->buffer + out->pos, str, to_write);
		out->pos += to_write;
		out->buffer[out->pos] = '\0';
	}
}

static int pipe_printf(pipe_t* out, const char *fmt, ...) {
	char printf_buf[1024] = {0};
	va_list args;
	int printed;

	va_start(args, fmt);
	printed = vsprintf(printf_buf, fmt, args);
	va_end(args);

	append_stdout_buffer(out, printf_buf, printed);

	return printed;
}

void system_command_handler(char* in) {
	bool should_break = false;
	command_received(in, &should_break, NULL, NULL);
}

void append_char(char** result, int* result_capacity, int* result_len, char c) {
    if (*result_len + 1 >= *result_capacity) {
        *result_capacity *= 2;
        *result = realloc(*result, *result_capacity);
    }
    (*result)[(*result_len)++] = c;
    (*result)[*result_len] = '\0';
}

char* execute_command_substitutions(char* command) {
    char* result = malloc(512);
    int result_capacity = 512;
    int result_len = 0;

    while (*command) {
        char* substitution = malloc(65);
        int substitution_capacity = 64;
        int substitution_len = 0;

        if (command[0] == '$' && command[1] == '(') {
            command += 2; // Skip past '$('

            char prev = '(';
            while (*command != ')' || prev == '\\') {
                prev = *command++;
                if (prev == '\\') {
                    continue;
                }

                append_char(&substitution, &substitution_capacity, &substitution_len, prev);
            }


            bool should_break;
            pipe_t* stdout_final_pipe = make_pipe(malloc(PIPE_BUFFER_SIZE), PIPE_BUFFER_SIZE, 0);
            command_received(substitution, &should_break, NULL, stdout_final_pipe);

            for (int j = 0; j < stdout_final_pipe->pos; j++) {
                append_char(&result, &result_capacity, &result_len, stdout_final_pipe->buffer[j]);
            }
             
            free(stdout_final_pipe->buffer);
            free(stdout_final_pipe);

            free(substitution);

            command++; // Skip past ')'
            
            if (result_len > 0 && result[result_len - 1] == '\n') {
                result[result_len - 1] = '\0';
                result_len--;
            }            
        } else {
            append_char(&result, &result_capacity, &result_len, *command++);
        }
    }

    return result;
}
			 

bool run_command(char* command, char** terminal_envp, bool* should_break, pipe_t* stdin_pipe, pipe_t** stdout_pipe) {
	pipe_t* outgoing_stdout = stdout_pipe ? *stdout_pipe : NULL;
	pipe_t* incoming_stdin = stdin_pipe;

    if (strncmp(command, (char*)"layout ", 7) == 0) {
		set_layout(command, outgoing_stdout);

	} else if (strncmp(command, (char*)"cd ", 3) == 0) {
		char** argv = argv_split(command);
		argv = argv_env_process(argv);

		cd(argv, outgoing_stdout);

		free_argv(argv);
	} else if (strncmp(command, (char*)"pwd", 3) == 0) {
		pwd(outgoing_stdout);
	} else if (strncmp(command, (char*)"export ", 7) == 0) {
        char* new_command = execute_command_substitutions(command);
        char* argv_str = read_env(new_command);
		export(argv_str, outgoing_stdout);
        free(new_command);
		free(argv_str);
	} else if (strncmp(command, (char*)"fault ", 6) == 0) {
		char* argv_str = read_env(command);
		fault(argv_str, outgoing_stdout);
		free(argv_str);
	} else if (strncmp(command, (char*)"read ", 5) == 0) {
		read_(command, outgoing_stdout);
	} else if (strcmp(command, (char*)"exit") == 0) {
		*should_break = true;
	} else {
        char* new_command = execute_command_substitutions(command);
		char** argv = argv_split(new_command);
		argv = argv_env_process(argv);

		int pid = spawn_process(argv, terminal_envp, outgoing_stdout, incoming_stdin);
		if (pid == -1) {
            free(new_command);
			free_argv(argv);
			return false;
		}

        free(new_command);
		free_argv(argv);
	}

	return true;
}

void set_layout(char* command, pipe_t* out) {
    if (command[7] == 0) {
		pipe_printf(out, "No keyboard layout specified!\n");
		return;
	} else {
		char* keymap_name = &command[7];
		pipe_printf(out, "Setting keyboard layout to %s\n", keymap_name);

		set_env(SYS_ENV_SET_LAYOUT, keymap_name);
	}
}

void cd(char** argv, pipe_t* out) {
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
			pipe_printf(out, "No root filesystem specified!\n");
			return;
		}
	} else if (argc == 2) {
		cancd = resolve(argv[1], path_buf);
	} else {
		pipe_printf(out, "Too many arguments.");
		return;
	}

	if (!cancd) {
		pipe_printf(out, "No such file or directory: %s\n", path_buf);
		return;
	}

	int fd = open(path_buf, FILE_OPEN_MODE_READ);
	if (fd != -1) {
		pipe_printf(out, "You can only change to a folder!\n");
		close(fd);
		return;
	}

	set_env(SYS_SET_PWD_ID, path_buf);
}

extern char** terminal_envp;

void export(char* command, pipe_t* out) {
	if (strlen(command) <= 7) {
		pipe_printf(out, "No environment variable specified! Try like this: export MYVAR=value\n");
		return;
	}

	char* env_var = command + 7;

	char* env_name_tmp = malloc(strlen(env_var) + 1); //Make a copy of the string to operate on it
	memset(env_name_tmp, 0, strlen(env_var) + 1);
	strcpy(env_name_tmp, env_var);
	
	char* env_name = strtok(env_name_tmp, "=");
	if (strcmp(env_var, env_name) == 0) {
		pipe_printf(out, "No environment variable value specified! Try like this: export MYVAR=value\n");
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

void fault(char* command, pipe_t* out) {
	if (strcmp("#pf", &command[6]) == 0) {
		uint8_t* ptr = (uint8_t*) ~0;
		*ptr = 69;
	} else if (strcmp("#de", &command[6]) == 0) {
		printf("%d\n", 1 / 0);
	} else if (strcmp("#gp", &command[6]) == 0) {
		asm volatile ("hlt");
	} else if (strcmp("help", &command[6]) == 0) {
		printf("fault #pf, #de, #gp");
	} else {
		pipe_printf(out, "Unknown fault: %s", &command[6]);
	}
}

void read_(char* command, pipe_t* out) {
	if (strlen(command) <= 5) {
		pipe_printf(out, "No env var specified! Try like this: read output_var_name\n");
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
	export(export_cmd, out);
}

void pwd(pipe_t* out) {
	GET_CWD(cwd);
	pipe_printf(out, "%s\n", cwd);
}

void set_wait_and_yield() {
    set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*)100);
	yield();
}

bool already_in_ipc = false;

int spawn_process(char** argv, char** terminal_envp, pipe_t* stdout, pipe_t* stdin) {
	char* executable = search_executable((char*) argv[0]);
	const char** envp = (const char**) terminal_envp;

	set_env(SYS_ENV_PIN, (void*) 1); // we dont want that the program executes until init_ipc is done. so just pin ourself until it is done
	yield();
	int pid = spawn(executable, (const char**) argv, envp);

	if (pid == -1) {
		set_env(SYS_ENV_PIN, (void*) 0);
		return -1;
	}

	if (stdout != NULL) {
		set_pipe(pid, stdout, PIPE_STDOUT);
	}

	if (stdin != NULL) {
		set_pipe(pid, stdin, PIPE_STDIN);
	}

	if (already_in_ipc) {
		// printf("WARNING: already_in_ipc == true\n");
		set_env(SYS_ENV_PIN, (void*) 0);
		goto normal_wait;
	}

	already_in_ipc = true;

	ipc_init_mapping(IPC_CONNECTION_TERMINAL, pid);
	set_env(SYS_ENV_PIN, (void*) 0);
	while (get_proc_info(pid)) {
		if (ipc_init_host(IPC_CONNECTION_TERMINAL)) {
			goto ipc_tunnel_ok;
		}
		set_wait_and_yield();
	}

	already_in_ipc = false;
	goto done;

ipc_tunnel_ok:
	while (get_proc_info(pid)) {
		char out[0x1000] = { 0 };
		if (ipc_message_ready(IPC_CONNECTION_TERMINAL, (void*) out)) {
			bool should_break = false;
			command_received(out, &should_break, NULL, NULL);
			ipc_ok(IPC_CONNECTION_TERMINAL);
		}
		set_wait_and_yield();
	}

	already_in_ipc = false;

normal_wait:
	while (get_proc_info(pid)) {
		set_wait_and_yield();
	}

done:
	free(executable);
	return pid;
}
