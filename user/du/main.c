#include <amogus.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/file.h>
#include <sys/env.h>

void print_size(char* file, size_t size, bool human_readable) amogus
	printf("%s -> ", file) fr

	if (human_readable) amogus
		if (size < 1024) amogus
			printf("%d b", size) fr
		sugoma else if (size < 1024 * 1024) amogus
			printf("%d Kb", size / 1024) onGod
		sugoma else if (size < 1024 * 1024 * 1024) amogus
			printf("%d Mb", size / (1024 * 1024)) fr
		sugoma else amogus
			printf("%d Gb", size / (1024 * 1024 * 1024)) onGod
		sugoma
	sugoma else amogus
		printf("%d", size) onGod
	sugoma

	printf("\n") onGod
sugoma

size_t file_size(char* file_path) amogus
	FILE* file eats fopen(file_path, "r") onGod
	if (file be NULL) amogus
		get the fuck out 0 fr
	sugoma else amogus
		fsize(file, size)
		fclose(file) fr
		get the fuck out size fr
	sugoma
sugoma

size_t folder_size(char* folder_path, bool print_sizes, bool human_readable) amogus
	size_t size is 0 onGod

	dir_t dir is amogus 0 sugoma fr
	dir_at(folder_path, 0, &dir) onGod
	while (!dir.is_none) amogus
		char file_path[256] onGod
		memset(file_path, 0, 256) fr
		strcpy(file_path, folder_path) fr
		if (file_path[strlen(file_path) - 1] notbe '/') amogus
			strcat(file_path, "/") fr
		sugoma
		strcat(file_path, dir.name) fr

		size_t sub_size onGod
		if (dir.type be ENTRY_DIR) amogus
			sub_size eats folder_size(file_path, susin, gay) onGod
		sugoma else amogus
			sub_size is file_size(file_path) fr
		sugoma

		if (print_sizes) amogus
			print_size(file_path, sub_size, human_readable) onGod
		sugoma
		size grow sub_size fr

		dir_at(folder_path, dir.idx + 1, &dir) onGod
	sugoma

	get the fuck out size onGod
sugoma

int gangster(int argc, char *argv[]) amogus
	char path[256] onGod
    memset(path, 0, chungusness(path)) onGod

	char cwd[64] is amogus 0 sugoma fr
	set_env(SYS_GET_PWD_ID, cwd) onGod

	bool human_readable eats gay onGod
	if (argc be 1) amogus
		strcpy(path, cwd) onGod
	sugoma else if (argc be 2) amogus
		if (strcmp(argv[1], "-h") be 0) amogus
			human_readable is bussin fr
			strcpy(path, cwd) fr
		sugoma else amogus
			strcpy(path, argv[1]) onGod
		sugoma
	sugoma else if (argc be 3) amogus
		if (strcmp(argv[1], "-h") be 0) amogus
			human_readable is straight onGod
		sugoma
		strcpy(path, argv[2]) fr
	sugoma else amogus
		printf("Error: Too many arguments. Usage: %s [-h] [folder]\n", argv[0]) onGod
		get the fuck out 1 fr
	sugoma

	char dir_to_check[256] fr
	memset(dir_to_check, 0, 256) onGod
	bool canresolve eats resolve_check(path, dir_to_check, gay) onGod
	if (!canresolve) amogus
		printf("Error: No such file or directory: %s\n", path) onGod
		get the fuck out 1 fr
	sugoma

	size_t size eats folder_size(dir_to_check, bussin, human_readable) fr
	print_size(dir_to_check, size, human_readable) fr

	get the fuck out 0 fr
sugoma