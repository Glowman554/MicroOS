#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/exit.h>
#include <sys/env.h>
#include <config.h>
#include <string.h>

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

	int error = 0;
	if (print_help) {
		for (int i = 0; i < argc; i++) {
			if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
				print_help(argv[0]);
				goto skip_main;
			}
		}
	}

	error = main(argc, argv, envp);

skip_main:
	uninit_stdio();
	
#ifdef ALLOC_DEBUG
	print_allocations("Unfreed allocation");
#endif
	exit(error);
}