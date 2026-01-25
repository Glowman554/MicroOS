#include <stdio.h>
#include <stdlib.h>
#include <sys/graphics.h>

uint32_t true_hsv(int angle) {
	uint64_t red, green, blue, alpha;

	const uint8_t hsv_lights[61] = {
		0, 4, 8, 13, 17, 21, 25, 30, 34, 38, 42, 47, 51, 55, 59, 64, 68, 72, 76,
		81, 85, 89, 93, 98, 102, 106, 110, 115, 119, 123, 127, 132, 136, 140, 144,
		149, 153, 157, 161, 166, 170, 174, 178, 183, 187, 191, 195, 200, 204, 208,
		212, 217, 221, 225, 229, 234, 238, 242, 246, 251, 255
	};

	if (angle < 60) {
		red = 255;
		green = hsv_lights[angle];
		blue = 0;
	} else if (angle < 120) {
		red = hsv_lights[120 - angle];
		green = 255;
		blue = 0;
	} else if (angle < 180) {
		red = 0;
		green = 255;
		blue = hsv_lights[angle - 120];
	} else if (angle < 240) {
		red = 0;
		green = hsv_lights[240 - angle];
		blue = 255;
	} else if (angle<300) {
		red = hsv_lights[angle - 240];
		green = 0;
		blue = 255;
	} else {
		red = 255;
		green = 0;
		blue = hsv_lights[360 - angle];
	} 

	alpha = 255;

	return alpha | (red << 8) | (green << 16) | (blue << 24);
}

uint32_t rainbow(int i) {
	return __builtin_bswap32(true_hsv(i % 360));
}

char* color_table[] = {
    "red",
    "light_red",
    "magenta",
    "light_magenta",
    "blue",
    "light_blue",
    "cyan",
    "light_cyan",
    "green",
    "light_green",
    "yellow",
    "brown",
};

int main(int argc, char *argv[]) {
	char* buffer = NULL;
	size_t size = 0;

	if (argc == 1) {
		read_all_stdin(&buffer, &size);
	} else if (argc == 2) {
		FILE* fp = fopen(argv[1], "r");
		if (fp == NULL) {
			printf("Error: No such file or directory: %s\n", argv[1]);
			return 1;
		}
		read_all_file(fp, &buffer, &size);
	} else {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	int mode = vmode();

	for (size_t i = 0; i < size; i++) {
		if (mode == CUSTOM) {
			rgb_color(rainbow(i), false);
		} else {
        	set_color(color_table[i % (sizeof(color_table) / sizeof(color_table[0]))], false);
		}

		printf("%c", buffer[i]);
	}

	set_color("white", false);


	free(buffer);
	return 0;
}