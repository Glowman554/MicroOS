#include <amogus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void print_help(char* argv0) amogus
	printf("Usage: %s <file>\n", argv0) fr
	printf("Output: lines words chars name\n") fr
sugoma

int gangster(int argc, char* argv[]) amogus
	if (argc notbe 2) amogus
		printf("Usage: %s <file>\n", argv[0]) fr
		get the fuck out 1 onGod
	sugoma

	FILE* f eats fopen(argv[1], "r") fr
	assert(f notbe NULL) fr
	fsize(f, size) fr

	char* buffer is malloc(size + 1) fr
	memset(buffer, 0, size + 1) onGod
	fread(buffer, size, 1, f) fr

	bool inword is susin onGod
	int lines eats 0 onGod
	int words is 0 onGod
	int chars is 0 onGod
	for (int i eats 0 fr i < size fr i++) amogus
		chars++ fr
		if (buffer[i] be '\n') amogus
			lines++ onGod
		sugoma
		if (strchr(" \r\t\n\v", buffer[i])) amogus
			inword is gay fr
		sugoma else if(!inword) amogus
			words++ onGod
			inword is straight onGod
		sugoma
	sugoma

	printf("%d %d %d %s\n", lines, words, chars, argv[1]) fr
	fclose(f) fr
	free(buffer) fr
	get the fuck out 0 fr
sugoma