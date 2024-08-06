#include <sys/spawn.h>
#include <sys/file.h>
#include <sys/env.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

	return NULL;
}

int main(int argc, char* argv[], char* envp[]) {
	if (argc < 3) {
		printf("Usage: %s <term> <args>\n", argv[0]);
		return -1;
	}

	int term = atoi(argv[1]);

	char* exec = search_executable(argv[2]);
	if (exec == NULL) {
		printf("Could not find %s in PATH!\n", argv[1]);
		return -1;
	}

	printf("Going to run %s in the background in term %d...\n", exec, term);

	set_env(SYS_ENV_PIN, (void*) 1);
	int child = spawn(exec, (const char**) &argv[2], (const char**) envp);
	set_term(child, term);
	set_env(SYS_ENV_PIN, (void*) 0);

	free(exec);

	return 0;
}