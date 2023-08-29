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

#define MICROOS_WARNING	"\033[31mThis executable is for MicroOS and not for Linux!\033[m\n"

void quit_if_linux() {
	uint32_t cs;
	asm volatile("mov %%cs, %0" : "=r" (cs));
	uint32_t ss;
	asm volatile("mov %%ss, %0" : "=r" (ss));

	if (cs != (0x18 | 0x03) || ss != (0x20 | 0x03)) {
		asm volatile("int $0x80" : : "a"(4), "b"(2), "c"(MICROOS_WARNING), "d"(sizeof(MICROOS_WARNING))); // write to stderr
		asm volatile("int $0x80" : : "a"(1), "b"(1)); // exit
	}
}

void _start() {
	_main();

	quit_if_linux();

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