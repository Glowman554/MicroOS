#include <buildin/graphics.h>
#include <stdint.h>
#include <sys/graphics.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PSF1_DRAW_CHAR_IMPL
#include <buildin/psf1.h>

uint8_t* fb;
int fb_mode;

fb_info_t fb_info;
void* buffer;
psf1_font_t fb_font;

bool fb_initialized = false;

void start_frame() {
	if (!fb_initialized) {
		fb_mode = vmode();

		if (fb_mode == TEXT_80x25) {
			fb = (uint8_t*) malloc(get_width() * get_height() * 2);
		} else {
			fb_info = fb_load_info();
			buffer = malloc(fb_info.fb_pitch * fb_info.fb_height);
			fb_info.fb_addr = (uint32_t) buffer;

			char* font = getenv("FONT");
			if (font) {
				fb_font = psf1_from_file(font);
			} else {
				printf("Please set $FONT\n");
				abort();
			}
		}

		fb_initialized = true;
	}

	if (fb_mode == TEXT_80x25) {
		memset(fb, 0, get_width() * get_height() * 2);
	} else {
		memset(buffer, 0x00, fb_info.fb_pitch * fb_info.fb_height);
	}
}

void end_frame() {
	if (fb_mode == TEXT_80x25) {
		vpoke(0, fb, get_width() * get_height() * 2);
	} else {
		vpoke(0, buffer, fb_info.fb_pitch * fb_info.fb_height);
	}
}

int get_width() {
	if (fb_mode == TEXT_80x25) {
		return 80;
	} else {
		return fb_info.fb_width / 8;
	}
}

int get_height() {
	if (fb_mode == TEXT_80x25) {
		return 25;
	} else {
		return fb_info.fb_height / 16;
	}
}

uint32_t color_translation_table[] = {
    0xFF000000, // black
    0xFF0000AA, // blue
    0xFF00AA00, // green
    0xFF00AAAA, // cyan
    0xFFAA0000, // red
    0xFFAA00AA, // magenta
    0xFFAA5500, // brown
    0xFFAAAAAA, // grey
    0xFF555555, // dark grey
    0xFF5555FF, // bright blue
    0xFF55FF55, // bright green
    0xFF00FFFF, // bright cyan
    0xFFFF5555, // bright red
    0xFFFF55FF, // bright magenta
    0xFFFFFF00, // yellow
    0xFFFFFFFF, // white
};

void draw_char(int x, int y, char c, int color) {
	if (fb_mode == TEXT_80x25) {
		fb[(y * get_width() + x) * 2] = c;
		fb[(y * get_width() + x) * 2 + 1] = color;
	} else {
		psf1_draw_char(&fb_info, &fb_font, x * 8, y * 16, c, color_translation_table[color], 0); // TODO
	}
}

void draw_string(int x, int y, char* str, int color) {
	int i = 0;
	while (str[i] != '\0') {
		draw_char(x + i, y, str[i], color);
		i++;
	}
}

void set_pixel(int x, int y, int color) {
	if (fb_mode == TEXT_80x25) {
		fb[(y * get_width() + x) * 2] = ' ';
		fb[(y * get_width() + x) * 2 + 1] = color;
	} else {
		printf("set_pixel not implemented\n");
		abort();
	}
}