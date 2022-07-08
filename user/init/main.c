#include <stdio.h>
#include <sys/spawn.h>
#include <sys/env.h>

#include <stddef.h>

int main(int argc, char* argv[], char* envp[]) {
	char* cwd = "initrd:/";
	set_env(SYS_SET_PWD_ID, cwd);

	char* new_argv[] = {
		"initrd:/bin/terminal.elf",
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