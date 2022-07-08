#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/file.h>
#include <sys/spawn.h>

int main(int argc, char* argv[], char* envp[]) {
	printf("Hello, world from a userspace program!\n");

	for (int i = 0; i < argc; i++) {
		printf("argv[%d] = %s\n", i, argv[i]);
	}

	for (int i = 0; envp[i]; i++) {
		printf("envp[%d] = %s\n", i, envp[i]);
	}

	FILE* file = fopen("initrd:/test.txt", "r");
	assert(file != NULL);
	fsize(file, len);
	char* buf = malloc(len + 1);
	fread(buf, 1, len, file);
	fclose(file);
	buf[len] = '\0';
	printf("%s\n", buf);
	free(buf);

	dir_t dir = { 0 };
	int i = 0;
	while (!dir.is_none) {
		memset(&dir, 0, sizeof(dir_t));
		dir_at("initrd:/", i++, &dir);
		if (!dir.is_none) {
			printf("%s (%d)\n", dir.name, dir.type);
		}
	}

	char* new_argv [] = { "initrd:/bin/test.elf", NULL };

	int pid = spawn("initrd:/bin/test.elf", (const char**) new_argv, (const char**) envp);
	printf("Spawned pid %d\n", pid);
	while (get_proc_info(pid)) {
		yield();
	}

    while(1) {
		printf("> ");

		int len = 0;
		char* in = (char*) malloc(128);
		memset(in, 0, 128);

		bool reading = true;

		while (reading) {
			in[len] = getchar();
			putchar(in[len]);
			if(in[len] == '\n') {
				in[len] = '\0';
				reading = false;
			} if (in[len] == '\b') {
				in[len] = '\0';
				len--;
			} else {
				len++;
			}
		}

		if (strcmp(in, "acs") == 0) {
			asm volatile("hlt");
		}

		if (strcmp(in, "about") == 0) {
			printf("MicroOS Copyright (C) 2022 Glowman554\n\n");
		}

		if (strcmp(in, "exit") == 0) {
			break;
		}

		if (strcmp(in, "help") == 0) {
			printf("about - Prints out the about message\n");
			printf("help - Prints out this help message\n");
			printf("acs - Causes an access violation\n");
			printf("exit - Exits the program\n");
		}

		free(in);
	}

	return 0;
}