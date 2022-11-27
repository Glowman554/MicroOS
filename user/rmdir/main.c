#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: %s <path>\n", argv[0]);
		return 1;
	}

	char* path = argv[1];

	char path_full[512] = { 0 };
	if (!resolve(path, path_full)) {
		printf("Directory does not exist\n");
		return 1;
	}

	delete_dir(path_full);

	return 0;
}