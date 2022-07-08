#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[], char* envp[]) {
	printf("Hello, world from a userspace program 2!\n");

	for (int i = 0; i < argc; i++) {
		printf("argv[%d] = %s\n", i, argv[i]);
	}

	for (int i = 0; envp[i]; i++) {
		printf("envp[%d] = %s\n", i, envp[i]);
	}

    return 0;
}