#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[], char* envp[]) {
	printf("Hello, world from a userspace program!\n");
	printf("THIS SHELL IS JUST FOR TESTING AND NOT THE FINAL SHELL!\n");

    while(1) {
		printf("> ");

		int len = 0;
		char in[100] = {0};
		bool reading = true;

		while (reading) {
			in[len] = getchar();
			putchar(in[len]);
			if(in[len] == '\n') {
				in[len] = '\0';
				reading = false;
			} else {
				len++;
			}
		}

		if (strcmp(in, "acces-violation") == 0) {
			asm volatile("hlt");
		}

		if (strcmp(in, "about") == 0) {
			printf("MicroOS Copyright (C) 2022 Glowman554\n\n");
		}

		if (strcmp(in, "help") == 0) {
			printf("about - Prints out the about message\n");
			printf("help - Prints out this help message\n");
			printf("acces-violation - Causes an access violation\n");
		}
	}
}