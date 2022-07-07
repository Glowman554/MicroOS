#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/exit.h>

int main(int argc, char* argv[], char* envp[]);

void _start() {
	char* EMPTY[] = {
		NULL
	};

	initialize_heap((void*) HEAP_ADDRESS, HEAP_PAGES);
	init_stdio();

	int error = main(0, EMPTY, EMPTY);
	
	exit(error);
}