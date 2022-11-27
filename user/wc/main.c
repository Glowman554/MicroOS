#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void print_help(char* argv0) {
	printf("Usage: %s <file>\n", argv0);
	printf("Output: lines words chars name\n");
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}

	FILE* f = fopen(argv[1], "r");
	assert(f != NULL);
	fsize(f, size);

	char* buffer = malloc(size + 1);
	memset(buffer, 0, size + 1);
	fread(buffer, size, 1, f);

	bool inword = false;
	int lines = 0;
	int words = 0;
	int chars = 0;
	for (int i = 0; i < size; i++) {
		chars++;
		if (buffer[i] == '\n') {
			lines++;
		}
		if (strchr(" \r\t\n\v", buffer[i])) {
			inword = false;
		} else if(!inword) {
			words++;
			inword = true;
		}
	}

	printf("%d %d %d %s\n", lines, words, chars, argv[1]);
	// fclose(f);
	return 0;
}