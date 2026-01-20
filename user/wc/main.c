#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help(char* argv0) {
	printf("Usage: %s <file>\n", argv0);
	printf("Output: lines words chars name\n");
}

int main(int argc, char* argv[]) {
	const char* name = NULL;
	
	char* buffer = NULL;
	size_t size = 0;

	if (argc == 1) {
		name = "-";
		read_all_stdin(&buffer, &size);
	} else if (argc == 2) {
		FILE* f = fopen(argv[1], "r");
		name = argv[1];
		if (!f) {
			printf("Usage: %s <file>\n", argv[0]);
			return 1;
		}
		read_all_file(f, &buffer, &size);
	} else {
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}


	bool inword = false;
	int lines = 0;
	int words = 0;
	int chars = 0;
	for (size_t i = 0; i < size; i++) {
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

	printf("%d %d %d %s\n", lines, words, chars, name);
	free(buffer);
	return 0;
}