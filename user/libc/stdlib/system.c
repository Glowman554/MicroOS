#include <stdlib.h>
#include <stdio.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/env.h>
#include <non-standard/buildin/path.h>

char* terminal_executable = NULL;

int system(const char* in) {
	if (!terminal_executable) {
		terminal_executable = search_executable("terminal");
	}

	if (!terminal_executable) {
		printf("Could not find terminal executable!\n");
		abort();
	}
	char** envp = (char**) env(SYS_GET_ENVP_ID);

	char* new_argv[] = {
		"terminal",
		"-e",
		(char*) in,
		NULL
	};

	int pid = spawn(terminal_executable, (const char**) new_argv, (const char**) envp);

	while (get_proc_info(pid)) {
        set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*)1000);
		yield();
	}

	return 0;
}