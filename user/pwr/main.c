#include <stdio.h>
#include <string.h>

#include <sys/env.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <off/reset>\n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "off") == 0) {
		env(SYS_PWR_SHUTDOWN_ID);
	} else if (strcmp(argv[1], "reset") == 0) {
		env(SYS_PWR_RESET_ID);
	} else {
		printf("Usage: %s <off/reset>\n", argv[0]);
		return 1;
	}

	return 0;
}