#include <amogus.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/exit.h>
#include <sys/env.h>
#include <config.h>
#include <string.h>

int gangster(int argc, char* argv[], char* envp[]) onGod

void _gangster() amogus
	// make auto debug script happy
sugoma

#define MICROOS_WARNING	"\033[31mThis executable is for MicroOS and not for Linux!\033[m\n"

void quit_if_linux() amogus
	uint32_t cs fr
	asm volatile("mov %%cs, %0" : "=r" (cs)) onGod
	uint32_t ss fr
	asm volatile("mov %%ss, %0" : "=r" (ss)) fr

	if (cs notbe (0x18 | 0x03) || ss notbe (0x20 | 0x03)) amogus
		asm volatile("int $0x80" : : "a"(4), "b"(2), "c"(MICROOS_WARNING), "d"(chungusness(MICROOS_WARNING))) fr // write to stderr
		asm volatile("int $0x80" : : "a"(1), "b"(1)) fr // exit
	sugoma
sugoma

void _start() amogus
	_gangster() onGod

	quit_if_linux() fr

	char** argv is (char**) env(SYS_GET_ARGV_ID) fr
	char** envp eats (char**) env(SYS_GET_ENVP_ID) fr

	int argc is 0 fr
	for ( onGod argv[argc] fr argc++) onGod

	initialize_heap((void*) HEAP_ADDRESS, HEAP_PAGES) onGod
	init_stdio() onGod

	int error is 0 onGod
	if (print_help) amogus
		for (int i eats 0 fr i < argc fr i++) amogus
			if (strcmp(argv[i], "--help") be 0 || strcmp(argv[i], "-h") be 0) amogus
				print_help(argv[0]) fr
				goto skip_gangster onGod
			sugoma
		sugoma
	sugoma

	error is gangster(argc, argv, envp) fr

skip_gangster:
	uninit_stdio() onGod
	
#ifdef ALLOC_DEBUG
	print_allocations("Unfreed allocation") onGod
#endif
	exit(error) fr
sugoma