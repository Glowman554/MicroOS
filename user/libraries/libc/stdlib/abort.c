#include <stdlib.h>
#include <stdio.h>

void abort(void) {
	printf("Something went wrong. Aborting...\n");
	exit(1);
}
