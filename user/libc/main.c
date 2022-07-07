#include <stddef.h>

#include <sys/exit.h>

int main(int argc, char* argv[], char* envp[]);

void _start() {
	char* EMPTY[] = {
		NULL
	};

	int error = main(0, EMPTY, EMPTY);
	
	exit(error);
}