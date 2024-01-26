#include <stdio.h>
#include <stdlib.h>

#include <file_scanner.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}

	FILE* f = fopen(argv[1], "r");
	if (!f) {
		printf("Could not open file %s\n", argv[1]);
		return 1;
	}
	
	fsize(f, size);
	void* buffer = malloc(size);
	fread(buffer, size, 1, f);

	file_scanner_t* file_scanner = find_file_scanner(buffer, size);

	if (file_scanner == NULL) {
		printf("Unable to identify file type\n");
	} else {
		char additional_information[512] = { 0 };
		file_scanner->get_information(buffer, size, additional_information, sizeof(additional_information));

		printf("%s: %s\n", file_scanner->name, additional_information);
	}

	fclose(f);
	free(buffer);
	return 0;
}