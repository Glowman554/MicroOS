#include <stdio.h>

#include <sys/file.h>

char getchar() {
	char input[1] = { 0 };
	read(STDIN, input, 1, 0);

	return input[0];
}

void putchar(char c) {
	char output[1];
	output[0] = c;
	write(STDOUT, output, 1, 0);
}