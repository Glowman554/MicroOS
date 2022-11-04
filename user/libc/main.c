#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/exit.h>
#include <sys/env.h>
#include <config.h>

int main(int argc, char* argv[], char* envp[]);

void _main() {
	// make auto debug script happy
}

void _start() {
	_main();

	char** argv = (char**) env(SYS_GET_ARGV_ID);
	char** envp = (char**) env(SYS_GET_ENVP_ID);

	int argc = 0;
	for (; argv[argc]; argc++);

	initialize_heap((void*) HEAP_ADDRESS, HEAP_PAGES);
	init_stdio();

	int error = main(argc, argv, envp);

	uninit_stdio();
	
#ifdef ALLOC_DEBUG
	print_allocations("Unfreed allocation");
#endif
	exit(error);
}