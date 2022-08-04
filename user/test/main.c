#include <sys/graphics.h>
#include <assert.h>

int main(int argc, char* argv[], char* envp[]) {
	assert(vmode() == TEXT_80x25);

	const char hw[] = "Hello World!";
	uint8_t color = 0x07;


	for (int i = 0; hw[i] != '\0'; i++) {
		vpoke(i * 2, (uint8_t*) &hw[i], 1);
		vpoke(i * 2 + 1, &color, 1);
	}

	return 0;
}