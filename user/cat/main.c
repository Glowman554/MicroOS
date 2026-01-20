#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	char* buffer = NULL;
	size_t size = 0;

	if (argc == 1) {
		read_all_stdin(&buffer, &size);
	} else if (argc == 2) {
		FILE* fp = fopen(argv[1], "r");
		if (fp == NULL) {
			printf("Error: No such file or directory: %s\n", argv[1]);
			return 1;
		}
		read_all_file(fp, &buffer, &size);
	} else {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	for (size_t i = 0; i < size; i++) {
		printf("%c", buffer[i]);
	}

	free(buffer);
	return 0;
}