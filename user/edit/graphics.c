#include <graphics.h>
#include <sys/graphics.h>
#include <assert.h>

int get_width() {
	assert(vmode() == TEXT_80x25);
	return 80;
}

int get_height() {
	assert(vmode() == TEXT_80x25);
	return 25;
}

void draw_char(int x, int y, char c, int color) {
	vpoke(2 * (y * get_width() + x), c);
	vpoke(2 * (y * get_width() + x) + 1, color);
}

void draw_string(int x, int y, char* str, int color) {
	int i = 0;
	while (str[i] != '\0') {
		draw_char(x + i, y, str[i], color);
		i++;
	}
}