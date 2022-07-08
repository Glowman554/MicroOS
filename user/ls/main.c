#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/file.h>
#include <sys/env.h>

int main(int argc, char *argv[]) {
	char ls_path[256] = {0};
	set_env(SYS_GET_PWD_ID, ls_path);

	if (argc == 2) {
		bool canresolve = resolve_check(argv[1], ls_path, true);
		if (!canresolve) {
			printf("Error: No such file or directory: %s\n", argv[1]);
			return 1;
		}
	}

	int fd = open(ls_path, FILE_OPEN_MODE_READ);
	if (fd != -1) {
		printf("Error: You can't list a file\n", ls_path);
		close(fd);
		return 1;
	}

	dir_t dir = {0};
	dir_at(ls_path, 0, &dir);
	if (dir.is_none) {
		return 0;
	}

	do {
		printf("%s (%s)\n", dir.name, dir.type == ENTRY_FILE ? "file" : "dir");
		dir_at(ls_path, dir.idx + 1, &dir);
	} while (!dir.is_none);

	return 0;
}