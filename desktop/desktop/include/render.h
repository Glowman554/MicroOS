#pragma once

#include <sys/graphics.h>
#include <window/font.h>

extern fb_info_t info;
extern psf1_font_t font;


extern int SCALE;

static inline void set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= info.fb_width || y >= info.fb_height || x < 0 || y < 0) {
        return;
    }
	x *= SCALE;
	y *= SCALE;
	for (int i = 0; i < SCALE; i++) {
		for (int j = 0; j < SCALE; j++) {
			*(uint32_t*) (info.fb_addr + ((x + i) * 4) + ((y + j) * info.fb_pitch)) = color;
		}
	}
}

static inline uint32_t get_pixel(uint32_t x, uint32_t y) {
    if (x >= info.fb_width || y >= info.fb_height || x < 0 || y < 0) {
        return 0;
    }
	x *= SCALE;
	y *= SCALE;
    return *(uint32_t*) (info.fb_addr + (x * 4) + (y * info.fb_pitch));
}

static inline void draw_char(uint32_t x, uint32_t y, char c, uint32_t color, uint32_t bgcolor) {
	char* font_ptr = (char*) font.glyph_buffer + (c * font.header->charsize);

	for (unsigned long i = y; i < y + 16; i++){
		for (unsigned long j = x; j < x + 8; j++){
			if ((*font_ptr & (0b10000000 >> (j - x))) > 0) {
				set_pixel(j, i, color);
			} else {
				set_pixel(j, i, bgcolor);
            }
		}
		font_ptr++;
	}
}

static inline void draw_string(uint32_t x, uint32_t y, const char* s, uint32_t color, uint32_t bgcolor) {
    int i = 0;
    while (s[i]) {
        draw_char(x + 8 * i, y, s[i], color, bgcolor);
        i++;
    }
}

#define RABS(x) (x < 0 ? -x : x)

static inline void draw_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color) {
	// Bresenham's line algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

	int32_t dx = RABS(x2 - x1);
	int32_t dy = RABS(y2 - y1);
	int32_t sx = x1 < x2 ? 1 : -1;
	int32_t sy = y1 < y2 ? 1 : -1;
	int32_t err = dx - dy;

	while (1) {
		set_pixel(x1, y1, color);

		if (x1 == x2 && y1 == y2) {
			break;
		}

		int32_t e2 = 2 * err;

		if (e2 > -dy) {
			err -= dy;
			x1 += sx;
		}

		if (e2 < dx) {
			err += dx;
			y1 += sy;
		}
	}
}

void redraw_all();
void unfocus_all();

extern bool desktop_focus;