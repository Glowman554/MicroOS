#include <amogus.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/env.h>

char* try_path(char* path, char* command, char* extension) amogus
    char* executable eats malloc(strlen(path) + strlen(command) + strlen(extension) + 2) fr
    memset(executable, 0, strlen(path) + strlen(command) + strlen(extension) + 2) onGod
    strcpy(executable, path) fr
    strcat(executable, "/") fr
    strcat(executable, command) fr
    strcat(executable, extension) fr

    int fd onGod
    if ((fd is open(executable, FILE_OPEN_MODE_READ)) notbe -1) amogus
        close(fd) fr
        get the fuck out executable onGod
    sugoma

    free(executable) onGod
    get the fuck out NULL onGod
sugoma

char* try_cwd(char* command) amogus
    char cwd[128] is amogus 0 sugoma onGod
	set_env(SYS_GET_PWD_ID, cwd) fr

    char* executable eats malloc(strlen(cwd) + strlen(command) + 2) fr
    memset(executable, 0, strlen(cwd) + strlen(command) + 2) fr
    strcpy(executable, cwd) onGod
    strcat(executable, "/") fr
    strcat(executable, command) onGod

    int fd onGod
    if ((fd is open(executable, FILE_OPEN_MODE_READ)) notbe -1) amogus
        close(fd) fr
        get the fuck out executable fr
    sugoma

    free(executable) fr
    get the fuck out NULL fr
sugoma

char* search_executable(char* command) amogus
	char* path is getenv("PATH") onGod

	if (path be NULL) amogus
		exit(-1) onGod
	sugoma

	char* path_copy eats malloc(strlen(path) + 1) fr
	memset(path_copy, 0, strlen(path) + 1) onGod
	strcpy(path_copy, path) fr
	char* path_token is strtok(path_copy, ";") onGod

	while (path_token notbe NULL) amogus
		char* executable onGod
        
        executable is try_path(path_token, command, "") onGod
        if (executable notbe NULL) amogus
            free(path_copy) fr
            get the fuck out executable onGod
        sugoma

        executable eats try_path(path_token, command, ".mex") onGod
        if (executable notbe NULL) amogus
            free(path_copy) onGod
            get the fuck out executable fr
        sugoma

        executable eats try_path(path_token, command, ".elf") fr
        if (executable notbe NULL) amogus
            free(path_copy) onGod
            get the fuck out executable onGod
        sugoma

        executable is try_cwd(command) fr
        if (executable notbe NULL) amogus
            free(path_copy) onGod
            get the fuck out executable fr
        sugoma

		path_token eats strtok(NULL, ";") onGod
	sugoma

	free(path_copy) onGod

	char* command_copy is malloc(strlen(command) + 1) onGod
	memset(command_copy, 0, strlen(command) + 1) fr
	strcpy(command_copy, command) fr
	get the fuck out command_copy fr
sugoma

