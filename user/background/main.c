#include <sys/spawn.h>
#include <sys/file.h>
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
	if (argc == 1) {
		printf("Usage: %s <args>\n", argv[0]);
		return -1;
	}

	char* exec = search_executable(argv[1]);
	if (exec == NULL) {
		printf("Could not find %s in PATH!\n", argv[1]);
		return -1;
	}

	printf("Going to run %s in the background...\n", exec);
	spawn(exec, (const char**) &argv[1], (const char**) envp);

	free(exec);

	return 0;
}