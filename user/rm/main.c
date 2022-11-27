#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/file.h>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}

	char file_to_delete[256];
	memset(file_to_delete, 0, 256);

	bool canresolve = resolve_check(argv[1], file_to_delete, true);
	if (!canresolve) {
		printf("Error: No such file or directory: %s\n", argv[1]);
		return 1;
	}

	int fd = open(file_to_delete, FILE_OPEN_MODE_WRITE);
	
	if (fd == -1) {
		printf("Error: Could not open file %s\n", file_to_delete);
		return 1;
	}

	delete(fd);

	return 0;
}