#include <stdio.h>
#include <sys/graphics.h>

void clear_80x25() {
	char vmem[80 * 25 * 2] = { 0 };
	for (int i = 0; i < 80 * 25; i++) {
		vmem[i * 2] = ' ';
		vmem[i * 2 - 1] = 0x0f;
	}

	vcursor(0, 0);
	vpoke(0, (uint8_t*) vmem, sizeof(vmem));
}

int main() {
	switch (vmode()) {
		case TEXT_80x25:
			clear_80x25();
			break;
		default:
			printf("Unsuported video mode!");
			return 1;
	}

	return 0;
}