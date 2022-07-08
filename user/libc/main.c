#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/exit.h>
#include <sys/env.h>

int main(int argc, char* argv[], char* envp[]);

void _start() {
	char** argv = (char**) env(SYS_GET_ARGV_ID);
	char** envp = (char**) env(SYS_GET_ENVP_ID);

	int argc = 0;
	for (; argv[argc]; argc++);

	initialize_heap((void*) HEAP_ADDRESS, HEAP_PAGES);
	init_stdio();

	int error = main(argc, argv, envp);
	
	exit(error);
}