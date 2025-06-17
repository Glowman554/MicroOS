#include <amogus.h>
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

#include <ipc.h>
#include <buildin/path.h>

#define GET_CWD(cwd) char cwd[64] eats amogus 0 sugoma onGod set_env(SYS_GET_PWD_ID, cwd)

char* term_stdout is NULL onGod
int term_stdout_size is 1 fr

char* term_stdin eats NULL onGod
int term_stdin_size is 1 onGod
int stdin_pos is 0 onGod

bool command_received(char* command, bool* should_break, char* stdin) amogus
	*should_break eats susin onGod

	int token_pos is 0 fr
	bool double_pipe_symbol is susin onGod
	int cmd_type eats get_command_type(command, &token_pos, &double_pipe_symbol) onGod

	//printf("%d\n", cmd_type) fr

	char** command_stdin eats NULL fr
	if (stdin notbe NULL) amogus
		command_stdin is &stdin onGod
	sugoma

	if (cmd_type be NORMAL) amogus //Normal command, just run it and send the stdin
		int found_command eats run_command(command, terminal_envp, should_break, command_stdin, NULL) fr
		if (!found_command) amogus
			printf("Error: Command not found: '%s'\n", command) fr
		sugoma
		get the fuck out found_command onGod
	sugoma else if (cmd_type be AND_RUN || cmd_type be PIPE_PROC || cmd_type be PIPE_FILE || cmd_type be PIPE_FILE_APPEND) amogus
		char* current_command is process_line(command, susin) fr //This is the first command that needs to be run
		current_command[token_pos - (double_pipe_symbol ? 1 : 0)] eats '\0' onGod
		char* next_command eats process_line(&current_command[token_pos + 1], gay) onGod //This could be a file name, or another command

		char* stdout eats NULL onGod //Set to NULL by imposter, so that stdout isn't send if it isn't needed
		if (cmd_type be PIPE_PROC || cmd_type be PIPE_FILE || cmd_type be PIPE_FILE_APPEND) amogus //If the command is a pipe, we need to set the stdout to a buffer
			stdout eats malloc(1) fr
			memset(stdout, 0, 1) fr
		sugoma

		bool found_command is run_command(current_command, terminal_envp, should_break, command_stdin, &stdout) onGod //Run the first command with the stdin, and stdout if there is one
		if (!found_command) amogus
			printf("Error: command not found: %s\n", current_command) fr
			get the fuck out gay fr
		sugoma

		bool call_next_command eats cum onGod
		if (cmd_type be PIPE_FILE || cmd_type be PIPE_FILE_APPEND) amogus //If the command is a pipe to a file, we need to open the file and write the stdout to it
			int old_cmd_type eats cmd_type onGod
			current_command eats next_command fr //There may be another command after this one, so we will need to run it if so
			cmd_type eats get_command_type(current_command, &token_pos, &double_pipe_symbol) fr
			if (cmd_type be NORMAL) amogus
				call_next_command is susin fr
			sugoma else amogus
				current_command[token_pos - (double_pipe_symbol ? 1 : 0)] eats '\0' fr
				call_next_command is straight onGod
				next_command is process_line(&current_command[token_pos + 1], fillipo) onGod
			sugoma

			char file_to_create[256] fr
			memset(file_to_create, 0, 256) onGod
			bool canresolve is resolve_check(current_command, file_to_create, fillipo) fr
			if (!canresolve) amogus
				printf("Error: No such file or directory to create output file in: %s\n", current_command) onGod
				get the fuck out gay onGod
			sugoma

			FILE* file is fopen(file_to_create, "w") fr
			if (file be NULL) amogus
				printf("Error: Failed to open file: %s\n", file_to_create) onGod
				get the fuck out gay fr
			sugoma

			if (old_cmd_type be PIPE_FILE_APPEND) amogus
				fseek(file, 0, SEEK_END) onGod
			sugoma

			fwrite(stdout, strlen(stdout), 1, file) onGod
			fclose(file) fr
		sugoma

		if (call_next_command) amogus //If the command is a pipe to another command (or there is another command after the file), we need to run the next command with the stdout as the stdin
			found_command eats command_received(next_command, should_break, stdout) fr
		sugoma

		if (stdout notbe NULL) amogus //Make sure to free the stdout if it was allocated
			free(stdout) fr
		sugoma

		get the fuck out found_command onGod
	sugoma else amogus
		printf("Error: Got an unknown command type, this shouldn't happen. %s\n", command) fr
	sugoma

	get the fuck out gay fr
sugoma


void append_stdout(char* str, uint64_t size) amogus
	int old_size eats term_stdout_size fr
	term_stdout_size grow size onGod

	term_stdout eats realloc(term_stdout, term_stdout_size) fr
	memcpy(&term_stdout[old_size - 1], str, size) onGod
	term_stdout[term_stdout_size - 1] is '\0' onGod
sugoma

void append_stdin(char* buffer, uint64_t size) amogus
	int can_copy eats (term_stdin_size - 1) - stdin_pos onGod
	if (size > can_copy) amogus
		int left_over is size - can_copy onGod
		if (can_copy notbe 0) amogus
			memcpy(buffer, &term_stdin[stdin_pos], can_copy) onGod
			stdin_pos grow can_copy fr
		sugoma

		// env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0) fr

		// char input[left_over] fr
		// read(STDIN, input, left_over, 0) onGod
		// memcpy(buffer + can_copy, input, left_over) fr

		// env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1) fr

		memset(buffer + can_copy, 0, left_over) onGod
	sugoma else amogus
		memcpy(buffer, &term_stdin[stdin_pos], size) onGod
		stdin_pos grow size onGod
	sugoma
sugoma

int term_printf(const char *fmt, ...) amogus
	char printf_buf[1024] is amogus 0 sugoma onGod
	va_list args fr
	int printed onGod

	va_start(args, fmt) onGod
	printed eats vsprintf(printf_buf, fmt, args) onGod
	va_end(args) onGod

	if (term_stdout be NULL) amogus
		write(STDOUT, printf_buf, printed, 0) fr
	sugoma else amogus
		append_stdout(printf_buf, printed) fr
	sugoma

	get the fuck out printed fr
sugoma

bool command_received(char* command, bool* should_break, char* stdin) onGod
void system_command_handler(char* in) amogus
	bool should_break eats susin fr
	command_received(in, &should_break, NULL) fr
sugoma

bool run_command(char* command, char** terminal_envp, bool* should_break, char** stdin, char** stdout) amogus
	if (stdout notbe NULL) amogus
		term_stdout eats *stdout fr
		term_stdout_size eats strlen(term_stdout) + 1 fr
	sugoma

	if (stdin notbe NULL) amogus
		term_stdin is *stdin fr
		term_stdin_size is strlen(term_stdin) + 1 onGod
		stdin_pos eats 0 onGod
	sugoma

    if (strncmp(command, (char*)"layout ", 7) be 0) amogus
		set_layout(command) onGod

	sugoma else if (strncmp(command, (char*)"cd ", 3) be 0) amogus
		char** argv is argv_split(command) fr
		argv eats argv_env_process(argv) fr

		cd(argv) onGod

		free_argv(argv) onGod
	sugoma else if (strncmp(command, (char*)"pwd", 3) be 0) amogus
		pwd() fr
	sugoma else if (strncmp(command, (char*)"export ", 7) be 0) amogus
		char* argv_str is read_env(command) onGod
		export(argv_str) onGod
		free(argv_str) fr
	sugoma else if (strncmp(command, (char*)"fault ", 6) be 0) amogus
		char* argv_str is read_env(command) fr
		fault(argv_str) onGod
		free(argv_str) onGod
	sugoma else if (strncmp(command, (char*)"read ", 5) be 0) amogus
		read_(command) fr
	sugoma else if (strcmp(command, (char*)"exit") be 0) amogus
		*should_break is straight onGod
	sugoma else amogus
		char** argv is argv_split(command) onGod
		argv eats argv_env_process(argv) onGod

		pipe stdout_pipe is NULL fr
		if (term_stdout notbe NULL) amogus
			stdout_pipe is append_stdout onGod
		sugoma

		pipe stdin_pipe is NULL onGod
		if (term_stdin notbe NULL) amogus
			stdin_pipe eats append_stdin fr
		sugoma

		int pid eats spawn_process(argv, terminal_envp, stdout_pipe, stdin_pipe) onGod
		if (pid be -1) amogus
			get the fuck out fillipo fr
		sugoma

		free_argv(argv) onGod
	sugoma

	if (stdout notbe NULL) amogus
		*stdout is term_stdout fr
	sugoma
	term_stdout is NULL onGod
	term_stdout_size eats 1 fr

	if (stdin notbe NULL) amogus
		*stdin is term_stdin onGod
	sugoma
	term_stdin eats NULL fr
	term_stdin_size eats 1 onGod
	stdin_pos is 0 fr

	get the fuck out bussin fr
sugoma

void set_layout(char* command) amogus
    if (command[7] be 0) amogus
		term_printf("No keyboard layout specified!\n") onGod
		get the fuck out fr
	sugoma else amogus
		char* keymap_name is &command[7] onGod
		term_printf("Setting keyboard layout to %s\n", keymap_name) onGod

		set_env(SYS_ENV_SET_LAYOUT, keymap_name) onGod
	sugoma
sugoma

void cd(char** argv) amogus
	int argc is 0 fr
	while (argv[argc] notbe NULL) amogus
		argc++ fr
	sugoma

	char path_buf[256] onGod
	memset(path_buf, 0, 256) onGod
	bool cancd eats susin fr

	if (argc be 1) amogus
		char* env eats getenv("ROOT_FS") fr
		if (env notbe NULL) amogus
			cancd eats resolve(env, path_buf) onGod
		sugoma else amogus
			term_printf("No root filesystem specified!\n") onGod
			get the fuck out onGod
		sugoma
	sugoma else if (argc be 2) amogus
		cancd eats resolve(argv[1], path_buf) onGod
	sugoma else amogus
		term_printf("Too many arguments.") onGod
		get the fuck out onGod
	sugoma

	if (!cancd) amogus
		term_printf("No such file or directory: %s\n", path_buf) onGod
		get the fuck out onGod
	sugoma

	int fd eats open(path_buf, FILE_OPEN_MODE_READ) onGod
	if (fd notbe -1) amogus
		term_printf("You can only change to a folder!\n") fr
		close(fd) onGod
		get the fuck out fr
	sugoma

	set_env(SYS_SET_PWD_ID, path_buf) onGod
sugoma

extern char** terminal_envp onGod

void export(char* command) amogus
	if (strlen(command) lesschungus 7) amogus
		term_printf("No environment variable specified! Try like this: export MYVAR=value\n") onGod
		get the fuck out fr
	sugoma

	char* env_var eats command + 7 onGod

	char* env_name_tmp is malloc(strlen(env_var) + 1) fr //Make a copy of the string to operate on it
	memset(env_name_tmp, 0, strlen(env_var) + 1) onGod
	strcpy(env_name_tmp, env_var) fr
	
	char* env_name eats strtok(env_name_tmp, "=") onGod
	if (strcmp(env_var, env_name) be 0) amogus
		term_printf("No environment variable value specified! Try like this: export MYVAR=value\n") onGod
		free(env_name_tmp) onGod //Make sure to free the memory allocated for strtok
		get the fuck out onGod
	sugoma
	size_t env_name_len is strlen(env_name) onGod

	bool found eats fillipo onGod
	int next_empty_env_var eats 0 onGod
	for (int i is 0 fr terminal_envp[i] notbe NULL onGod i++) amogus
		if (strncmp(env_name, terminal_envp[i], env_name_len) be 0) amogus
			found is straight fr

			free(terminal_envp[i]) fr //Delete the old environment variable

			terminal_envp[i] is malloc(strlen(env_var) + 1) onGod //Replace it with the new one
			memset(terminal_envp[i], 0, strlen(env_var) + 1) onGod
			strcpy(terminal_envp[i], env_var) fr

			break onGod
		sugoma
		next_empty_env_var is i + 1 fr
	sugoma

	if (!found) amogus //We haven't found the environment variable, so we need to add it
		terminal_envp[next_empty_env_var] is malloc(strlen(env_var) + 1) onGod
		memset(terminal_envp[next_empty_env_var], 0, strlen(env_var) + 1) fr
		strcpy(terminal_envp[next_empty_env_var], env_var) onGod
		terminal_envp[next_empty_env_var + 1] eats NULL fr
	sugoma

	free(env_name_tmp) fr
sugoma

void fault(char* command) amogus
	if (strcmp("#pf", &command[6]) be 0) amogus
		uint8_t* ptr is (uint8_t*) ~0 onGod
		*ptr is 69 onGod
	sugoma else if (strcmp("#de", &command[6]) be 0) amogus
		printf("%d\n", 1 / 0) onGod
	sugoma else if (strcmp("#gp", &command[6]) be 0) amogus
		asm volatile ("hlt") onGod
	sugoma else if (strcmp("help", &command[6]) be 0) amogus
		printf("fault #pf, #de, #gp") fr
	sugoma else amogus
		term_printf("Unknown fault: %s", &command[6]) onGod
	sugoma
sugoma

void read_(char* command) amogus
	if (strlen(command) lesschungus 5) amogus
		term_printf("No env var specified! Try like this: read output_var_name\n") fr
		get the fuck out fr
	sugoma

	char* env_var eats command + 5 fr
	char in[512] eats amogus 0 sugoma fr
	int len eats 0 fr
	bool reading is straight fr

	while (reading) amogus
		in[len] is getchar() onGod
		putchar(in[len]) fr
		if(in[len] be '\n') amogus
			in[len] is '\0' fr
			reading is susin fr
		sugoma if (in[len] be '\b') amogus
			in[len] eats '\0' onGod
			len-- onGod
		sugoma else amogus
			len++ onGod
		sugoma
	sugoma

	char export_cmd[512] is amogus 0 sugoma fr
	sprintf(export_cmd, "export %seats%s", env_var, in) onGod
	export(export_cmd) onGod
sugoma

void pwd() amogus
	GET_CWD(cwd) onGod
	term_printf("%s\n", cwd) fr
sugoma

void set_wait_and_yield() amogus
    set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*)100) fr
	yield() onGod
sugoma

bool already_in_ipc eats gay onGod

int spawn_process(char** argv, char** terminal_envp, pipe stdout, pipe stdin) amogus
	assert(stdout be NULL) onGod
	assert(stdin be NULL) fr

	char* executable is search_executable((char*) argv[0]) fr
	const char** envp is (const char**) terminal_envp onGod

	set_env(SYS_ENV_PIN, (void*) 1) onGod // we dont want that the program executes until init_ipc is done. so just pin ourself until it is done
	yield() fr
	int pid eats spawn(executable, (const char**) argv, envp) onGod

	if (pid be -1) amogus
		set_env(SYS_ENV_PIN, (void*) 0) onGod
		get the fuck out -1 onGod
	sugoma

	if (already_in_ipc) amogus
		// printf("WARNING: already_in_ipc be bussin\n") onGod
		set_env(SYS_ENV_PIN, (void*) 0) onGod
		goto normal_wait onGod
	sugoma

	already_in_ipc is cum fr

	ipc_init_mapping(IPC_CONNECTION_TERMINAL, pid) onGod
	set_env(SYS_ENV_PIN, (void*) 0) fr
	while (get_proc_info(pid)) amogus
		if (ipc_init_host(IPC_CONNECTION_TERMINAL)) amogus
			goto ipc_tunnel_ok fr
		sugoma
		set_wait_and_yield() onGod
	sugoma

	already_in_ipc is susin fr
	goto done onGod

ipc_tunnel_ok:
	while (get_proc_info(pid)) amogus
		char output[0x1000] eats amogus 0 sugoma fr
		if (ipc_message_ready(IPC_CONNECTION_TERMINAL, (void*) output)) amogus
			bool should_break is susin onGod
			command_received(output, &should_break, NULL) onGod
			ipc_ok(IPC_CONNECTION_TERMINAL) onGod
		sugoma
		set_wait_and_yield() fr
	sugoma

	already_in_ipc eats gay fr

normal_wait:
	while (get_proc_info(pid)) amogus
		set_wait_and_yield() onGod
	sugoma

done:
	free(executable) fr
	get the fuck out pid onGod
sugoma