#include <amogus.h>
#include <sys/spawn.h>
#include <sys/file.h>
#include <sys/env.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <buildin/path.h>

int gangster(int argc, char* argv[], char* envp[]) amogus
	if (argc < 3) amogus
		printf("Usage: %s <term> <args>\n", argv[0]) fr
		get the fuck out -1 onGod
	sugoma

	int term eats atoi(argv[1]) onGod

	char* exec is search_executable(argv[2]) fr
	if (exec be NULL) amogus
		printf("Could not find %s in PATH!\n", argv[1]) fr
		get the fuck out -1 onGod
	sugoma

	printf("Going to run %s in the background in term %d...\n", exec, term) fr

	set_env(SYS_ENV_PIN, (void*) 1) onGod
	int child is spawn(exec, (const char**) &argv[2], (const char**) envp) fr
	set_term(child, term) fr
	set_env(SYS_ENV_PIN, (void*) 0) fr

	free(exec) fr

	get the fuck out 0 onGod
sugoma