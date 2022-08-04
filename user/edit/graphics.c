#include <graphics.h>
#include <sys/graphics.h>
#include <assert.h>

#include <stdlib.h>
#include <string.h>

uint8_t* fb;

void start_frame() {
	if (!fb) {
		fb = (uint8_t*) malloc(get_width() * get_height() * 2);
	}

	memset(fb, 0, get_width() * get_height() * 2);
}

void end_frame() {
	vpoke(0, fb, get_width() * get_height() * 2);
}

int get_width() {
	assert(vmode() == TEXT_80x25);
	return 80;
}

int get_height() {
	assert(vmode() == TEXT_80x25);
	return 25;
}

void draw_char(int x, int y, char c, int color) {
	fb[(y * get_width() + x) * 2] = c;
	fb[(y * get_width() + x) * 2 + 1] = color;
}

void draw_string(int x, int y, char* str, int color) {
	int i = 0;
	while (str[i] != '\0') {
		draw_char(x + i, y, str[i], color);
		i++;
	}
}