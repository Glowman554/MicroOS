#include <non-standard/sys/spawn.h>
#include <non-standard/sys/file.h>
#include <non-standard/sys/env.h>
#include <stddef.h>
#include <non-standard/stdio.h>
#include <stdlib.h>
#include <non-standard/buildin/path.h>

// #define BACKGROUND_DEBUG

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

#ifdef BACKGROUND_DEBUG
	printf("Going to run %s in the background in term %d...\n", exec, term);
#endif

	spawn_params_t params = {
		.path = exec,
		.argv = (const char**) &argv[2],
		.envp = (const char**) envp,
		.stdout = NULL,
		.stdin = NULL,
		.stderr = NULL,
		.term = term
	};

	spawn_param(&params);

	free(exec);

	return 0;
}