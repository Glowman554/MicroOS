#include <sys/spawn.h>
#include <sys/file.h>
#include <sys/env.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <buildin/path.h>

int main(int argc, char* argv[], char* envp[]) {
	if (argc < 3) {
		printf("Usage: %s <term> <args>\n", argv[0]);
		return -1;
	}

	int term = atoi(argv[1]);

	char* exec = search_executable(argv[2]);
	if (exec == NULL) {
		printf("Could not find %s in PATH!\n", argv[1]);
		return -1;
	}

	printf("Going to run %s in the background in term %d...\n", exec, term);

	set_env(SYS_ENV_PIN, (void*) 1);
	int child = spawn(exec, (const char**) &argv[2], (const char**) envp);
	set_term(child, term);
	set_env(SYS_ENV_PIN, (void*) 0);

	free(exec);

	return 0;
}