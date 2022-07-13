#include <stdio.h>
#include <sys/spawn.h>
#include <sys/env.h>

#include <stddef.h>

#define ROOT_FS "initrd:/"
#define TERMINAL "initrd:/bin/terminal.elf"

int main(int argc, char* argv[], char* envp[]) {
	char* cwd = "initrd:/";
	set_env(SYS_SET_PWD_ID, cwd);

	char* autostart = "startup.msh";
	FILE* f = fopen(autostart, "r");
	if (f) {
		fclose(f);
		char* new_argv[] = {
			TERMINAL,
			autostart,
			NULL
		};

		int pid = spawn(new_argv[0], (const char**) new_argv, (const char**) envp);

		while (get_proc_info(pid)) {
			yield();
		}
	} else {
		printf("Could not find autostart file: %s\n", autostart);
	}

	char* new_argv[] = {
		TERMINAL,
		NULL
	};

	while (true) {
		int pid = spawn(new_argv[0], (const char**) new_argv, (const char**) envp);

		while (get_proc_info(pid)) {
			yield();
		}

		printf("Terminal proccess %d exited\n", pid);
	}

	return 0;
}