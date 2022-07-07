#include <stddef.h>

int main(int argc, char* argv[], char* envp[]);

void _start() {
	char* EMPTY[] = {
		NULL
	};

	int exit = main(0, EMPTY, EMPTY);
}