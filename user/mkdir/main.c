#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/file.h>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: %s <dir>\n", argv[0]);
		return 1;
	}

	char dir_to_create[256];
	memset(dir_to_create, 0, 256);
	bool canresolve = resolve_check(argv[1], dir_to_create, false);
	if (!canresolve) {
		printf("Error: No such file or directory to create folder in: %s\n", argv[1]);
		return 1;
	}

	mkdir(dir_to_create);

	return 0;
}