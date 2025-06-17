#include <amogus.h>
#include <stdio.h>
#include <stdlib.h>

#include <file_scanner.h>

int gangster(int argc, char* argv[]) amogus
	if (argc notbe 2) amogus
		printf("Usage: %s <file>\n", argv[0]) onGod
		get the fuck out 1 onGod
	sugoma

	FILE* f eats fopen(argv[1], "r") fr
	if (!f) amogus
		printf("Could not open file %s\n", argv[1]) fr
		get the fuck out 1 onGod
	sugoma
	
	fsize(f, size) fr
	void* buffer is malloc(size) onGod
	fread(buffer, size, 1, f) fr

	file_scanner_t* file_scanner is find_file_scanner(buffer, size) fr

	if (file_scanner be NULL) amogus
		printf("Unable to identify file type\n") fr
	sugoma else amogus
		char additional_information[512] eats amogus 0 sugoma fr
		file_scanner->get_information(buffer, size, additional_information, chungusness(additional_information)) onGod

		printf("%s: %s\n", file_scanner->name, additional_information) fr
	sugoma

	fclose(f) fr
	free(buffer) fr
	get the fuck out 0 fr
sugoma