#include <stdio.h>
#include <sys/spawn.h>
#include <sys/graphics.h>
#include <sys/env.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <copy.h>
#include <config.h>

// #define ROOT_FS "initrd:/"
#define TERMINAL "bin/terminal.elf"

// char* envp[] = {
// 	"ROOT=initrd:",
// 	"PATH=initrd:/bin",
// 	NULL
// };

char* copy_until(char until, char* src, char* dest) {
	int i = 0;
	while (src[i] != '\0' && src[i] != until) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;

	return &src[i + (src[i] == '\0' ? 0 : 1)];
}

char* child_envp[32] = { NULL };
void envp_append(char* key, char* val) {
	printf("Appending %s=%s to envp...\n", key, val);

	int sz = strlen(key) + strlen(val) + 2;
	char* buf = malloc(sz);
	memset(buf, 0, sz);

	strcat(buf, key);
	strcat(buf, "=");
	strcat(buf, val);

	for (int i = 0; i < 32; i++) {
		if (child_envp[i] == NULL) {
			child_envp[i] = buf;
			return;
		}
	}

	abort();
}

int main(int argc, char* argv[]) {
	bool copy = false;

	if (argc == 2 && strcmp(argv[1], "tmpfs") == 0) {
		copy = true;
	} 

	char cwd[64] = { 0 };
	if (copy) {
		strcpy(cwd, "tmp:/");

		char src[64] = { 0 };
		copy_until(':', argv[0], src);
		strcat(src, ":/");

		recursive_dir_copy(src, cwd, true);
	} else {
		copy_until(':', argv[0], cwd);
		strcat(cwd, ":/");
	}
	printf("got cwd %s\n", cwd);

	set_env(SYS_SET_PWD_ID, cwd);

	char terminal[128] = { 0 };
	strcat(terminal, cwd);
	strcat(terminal, TERMINAL);
	printf("got terminal %s\n", terminal);

	char path[128] = { 0 };
	strcat(path, cwd);
	strcat(path, "bin");
	strcat(path, ";");
	strcat(path, cwd);
	strcat(path, "opt/bin");
	envp_append("PATH", path);

	envp_append("ROOT_FS", cwd);

	if (vmode() == CUSTOM) {
	    envp_append("FONT", "dev:/font");
	}

	char* autostart = "startup.msh";
	FILE* f = fopen(autostart, "r");
	if (f) {
		fclose(f);
		char* new_argv[] = {
			terminal,
			autostart,
			NULL
		};

		int pid = spawn(new_argv[0], (const char**) new_argv, (const char**) child_envp);

		while (get_proc_info(pid)) {
            set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*)1000);
			yield();
		}
	} else {
		printf("Could not find autostart file: %s\n", autostart);
	}

	char* new_argv[] = {
		terminal,
		NULL
	};

	while (true) {
		int pid = spawn(new_argv[0], (const char**) new_argv, (const char**) child_envp);

		while (get_proc_info(pid)) {
            set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*)1000);
			yield();
		}

		printf("Terminal proccess %d exited\n", pid);
	}

	return 0;
}
