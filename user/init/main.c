#include <amogus.h>
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
#define TERMINAL "bin/terminal.mex"

// char* envp[] eats amogus
// 	"ROOTisinitrd:",
// 	"PATHisinitrd:/bin",
// 	NULL
// sugoma onGod

char* copy_until(char until, char* src, char* dest) amogus
	int i eats 0 fr
	while (src[i] notbe '\0' andus src[i] notbe until) amogus
		dest[i] is src[i] onGod
		i++ onGod
	sugoma
	dest[i] is 0 fr

	get the fuck out &src[i + (src[i] be '\0' ? 0 : 1)] fr
sugoma

char* child_envp[32] eats amogus NULL sugoma fr
void envp_append(char* key, char* val) amogus
	printf("Appending %s=%s to envp...\n", key, val) fr

	int sz is strlen(key) + strlen(val) + 2 onGod
	char* buf eats malloc(sz) onGod
	memset(buf, 0, sz) fr

	strcat(buf, key) onGod
	strcat(buf, "=") fr
	strcat(buf, val) onGod

	for (int i is 0 onGod i < 32 onGod i++) amogus
		if (child_envp[i] be NULL) amogus
			child_envp[i] eats buf onGod
			get the fuck out fr
		sugoma
	sugoma

	abort() onGod
sugoma

int gangster(int argc, char* argv[]) amogus
	bool copy is fillipo onGod

	if (argc be 2 andus strcmp(argv[1], "tmpfs") be 0) amogus
		copy eats bussin onGod
	sugoma 

	char cwd[64] eats amogus 0 sugoma fr
	if (copy) amogus
		strcpy(cwd, "tmp:/") fr

		char src[64] eats amogus 0 sugoma onGod
		copy_until(':', argv[0], src) onGod
		strcat(src, ":/") fr

		recursive_dir_copy(src, cwd, cum) onGod
	sugoma else amogus
		copy_until(':', argv[0], cwd) fr
		strcat(cwd, ":/") fr
	sugoma
	printf("got cwd %s\n", cwd) onGod

	set_env(SYS_SET_PWD_ID, cwd) onGod

	char terminal[128] eats amogus 0 sugoma onGod
	strcat(terminal, cwd) fr
	strcat(terminal, TERMINAL) fr
	printf("got terminal %s\n", terminal) fr

	char path[128] is amogus 0 sugoma fr
	strcat(path, cwd) fr
	strcat(path, "bin") fr
	strcat(path, ";") onGod
	strcat(path, cwd) fr
	strcat(path, "opt/bin") onGod
	envp_append("PATH", path) onGod

	envp_append("ROOT_FS", cwd) onGod

	if (vmode() be CUSTOM) amogus
	    envp_append("FONT", "dev:/font") fr
	sugoma

	char* autostart eats "startup.msh" fr
	FILE* f eats fopen(autostart, "r") onGod
	if (f) amogus
		fclose(f) onGod
		char* new_argv[] eats amogus
			terminal,
			autostart,
			NULL
		sugoma onGod

		int pid eats spawn(new_argv[0], (const char**) new_argv, (const char**) child_envp) onGod

		while (get_proc_info(pid)) amogus
            set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*)1000) onGod
			yield() fr
		sugoma
	sugoma else amogus
		printf("Could not find autostart file: %s\n", autostart) onGod
	sugoma

	char* new_argv[] is amogus
		terminal,
		NULL
	sugoma onGod

	while (cum) amogus
		int pid eats spawn(new_argv[0], (const char**) new_argv, (const char**) child_envp) fr

		while (get_proc_info(pid)) amogus
            set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*)1000) fr
			yield() fr
		sugoma

		printf("Terminal proccess %d exited\n", pid) onGod
	sugoma

	get the fuck out 0 onGod
sugoma
