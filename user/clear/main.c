#include <stdio.h>
#include <sys/graphics.h>
#include <sys/file.h>

void clear_80x25() {
	char vmem[80 * 25 * 2] = { 0 };
	for (int i = 0; i < 80 * 25; i++) {
		vmem[i * 2] = ' ';
		vmem[i * 2 - 1] = 0x0f;
	}

	vcursor(0, 0);
	vpoke(0, (uint8_t*) vmem, sizeof(vmem));
}

void clear_custom() {
	int fd = open("dev:fst", FILE_OPEN_MODE_WRITE);
	if (fd == -1) {
		printf("Unsupported video mode!\n");
		return;
	}
	char cmd = 0;
	write(fd, &cmd, 1, 0);
	close(fd);
}

int main() {
	switch (vmode()) {
		case TEXT_80x25:
			clear_80x25();
			break;
		case CUSTOM:
			clear_custom();
			break;
		default:
			printf("Unsupported video mode!\n");
			return 1;
	}

	return 0;
}