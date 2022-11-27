#include <stdio.h>
#include <stddef.h>

int main(int argc, char* argv[], char* envp[]) {
	int idx = 0;
	while (envp[idx] != NULL) {
		printf("%s\n", envp[idx++]);
	}
	return 0;
}