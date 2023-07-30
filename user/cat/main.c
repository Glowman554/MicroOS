#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE* fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("Error: No such file or directory: %s\n", argv[1]);
		return 1;
	}

	fsize(fp, file_size);
	if (file_size == 0) {
		fclose(fp);
		return;
	}
	
	void* buffer = malloc(file_size);
	fread(buffer, file_size, 1, fp);

	fclose(fp);

	char* str = (char*) buffer;
	for (int i = 0; i < file_size; i++) {
		printf("%c", str[i]);
	}

	free(buffer);

	return 0;
}