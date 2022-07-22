#include <sys/graphics.h>
#include <assert.h>

int main(int argc, char* argv[], char* envp[]) {
	assert(vmode() == TEXT_80x25);

	const char hw[] = "Hello World!";

	for (int i = 0; hw[i] != '\0'; i++) {
		vpoke(i * 2, hw[i]);
		vpoke(i * 2 + 1, 0x07);
	}
	
	return 0;
}