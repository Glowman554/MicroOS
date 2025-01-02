#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/env.h>

char* try_path(char* path, char* command, char* extension) {
    char* executable = malloc(strlen(path) + strlen(command) + strlen(extension) + 2);
    memset(executable, 0, strlen(path) + strlen(command) + strlen(extension) + 2);
    strcpy(executable, path);
    strcat(executable, "/");
    strcat(executable, command);
    strcat(executable, extension);

    int fd;
    if ((fd = open(executable, FILE_OPEN_MODE_READ)) != -1) {
        close(fd);
        return executable;
    }

    free(executable);
    return NULL;
}

char* try_cwd(char* command) {
    char cwd[128] = { 0 };
	set_env(SYS_GET_PWD_ID, cwd);

    char* executable = malloc(strlen(cwd) + strlen(command) + 2);
    memset(executable, 0, strlen(cwd) + strlen(command) + 2);
    strcpy(executable, cwd);
    strcat(executable, "/");
    strcat(executable, command);

    int fd;
    if ((fd = open(executable, FILE_OPEN_MODE_READ)) != -1) {
        close(fd);
        return executable;
    }

    free(executable);
    return NULL;
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
		char* executable;
        
        executable = try_path(path_token, command, "");
        if (executable != NULL) {
            free(path_copy);
            return executable;
        }

        executable = try_path(path_token, command, ".mex");
        if (executable != NULL) {
            free(path_copy);
            return executable;
        }

        executable = try_path(path_token, command, ".elf");
        if (executable != NULL) {
            free(path_copy);
            return executable;
        }

        executable = try_cwd(command);
        if (executable != NULL) {
            free(path_copy);
            return executable;
        }

		path_token = strtok(NULL, ";");
	}

	free(path_copy);

	char* command_copy = malloc(strlen(command) + 1);
	memset(command_copy, 0, strlen(command) + 1);
	strcpy(command_copy, command);
	return command_copy;
}

