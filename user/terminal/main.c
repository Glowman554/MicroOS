#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
	printf("Hello, world from a userspace program!\n");

	FILE* file = fopen("initrd:/test.txt", "r");
	fseek(file, 0, SEEK_END);
	int len = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* buf = malloc(len + 1);
	fread(buf, 1, len, file);
	fclose(file);
	buf[len] = '\0';
	printf("%s\n", buf);
	free(buf);

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