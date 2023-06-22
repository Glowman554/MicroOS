#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/file.h>
#include <sys/env.h>

void print_size(char* file, size_t size, bool human_readable) {
	printf("%s -> ", file);

	if (human_readable) {
		if (size < 1024) {
			printf("%d b", size);
		} else if (size < 1024 * 1024) {
			printf("%d Kb", size / 1024);
		} else if (size < 1024 * 1024 * 1024) {
			printf("%d Mb", size / (1024 * 1024));
		} else {
			printf("%d Gb", size / (1024 * 1024 * 1024));
		}
	} else {
		printf("%d", size);
	}

	printf("\n");
}

size_t file_size(char* file_path) {
	FILE* file = fopen(file_path, "r");
	if (file == NULL) {
		return 0;
	} else {
		fsize(file, size)
		fclose(file);
		return size;
	}
}

size_t folder_size(char* folder_path, bool print_sizes, bool human_readable) {
	size_t size = 0;

	dir_t dir = {0};
	dir_at(folder_path, 0, &dir);
	while (!dir.is_none) {
		char file_path[256];
		memset(file_path, 0, 256);
		strcpy(file_path, folder_path);
		if (file_path[strlen(file_path) - 1] != '/') {
			strcat(file_path, "/");
		}
		strcat(file_path, dir.name);

		size_t sub_size;
		if (dir.type == ENTRY_DIR) {
			sub_size = folder_size(file_path, false, false);
		} else {
			sub_size = file_size(file_path);
		}

		if (print_sizes) {
			print_size(file_path, sub_size, human_readable);
		}
		size += sub_size;

		dir_at(folder_path, dir.idx + 1, &dir);
	}

	return size;
}

int main(int argc, char *argv[]) {
	char path[256];
    memset(path, 0, sizeof(path));

	char cwd[64] = { 0 };
	set_env(SYS_GET_PWD_ID, cwd);

	bool human_readable = false;
	if (argc == 1) {
		strcpy(path, cwd);
	} else if (argc == 2) {
		if (strcmp(argv[1], "-h") == 0) {
			human_readable = true;
			strcpy(path, cwd);
		} else {
			strcpy(path, argv[1]);
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "-h") == 0) {
			human_readable = true;
		}
		strcpy(path, argv[2]);
	} else {
		printf("Error: Too many arguments. Usage: %s [-h] [folder]\n", argv[0]);
		return 1;
	}

	char dir_to_check[256];
	memset(dir_to_check, 0, 256);
	bool canresolve = resolve_check(path, dir_to_check, false);
	if (!canresolve) {
		printf("Error: No such file or directory: %s\n", path);
		return 1;
	}

	size_t size = folder_size(dir_to_check, true, human_readable);
	print_size(dir_to_check, size, human_readable);

	return 0;
}