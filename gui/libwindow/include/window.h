#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <sys/graphics.h>
#include <sys/mouse.h>
#include <window/font.h>

typedef struct window {
    char name[64];

    int window_height;
    int window_width;

    int window_x;
    int window_y;

    fb_info_t info;

    int scale;

	bool redrawn;
} window_t;

extern window_t* window;

static inline void set_pixel_window(uint32_t x, uint32_t y, uint32_t color) {
	if (x > window->window_width || y > window->window_height || x < 0 || y < 0) {
		return;
	}
    if (x + window->window_x >= window->info.fb_width || y + window->window_y >= window->info.fb_height || x < 0 || y < 0) {
		return;
    }

    x *= window->scale;
    y *= window->scale;
    x += window->window_x * window->scale;
    y += window->window_y * window->scale;

    for (int i = 0; i < window->scale; i++) {
        for (int j = 0; j < window->scale; j++) {
            *(uint32_t*) (window->info.fb_addr + ((x + i) * 4) + ((y + j) * window->info.fb_pitch)) = color;
        }
    }
}

static inline uint32_t get_pixel_window(uint32_t x, uint32_t y) {
	if (x + window->window_x >= window->info.fb_width || y + window->window_y >= window->info.fb_height || x < 0 || y < 0) {
    	return 0;
	}

	x *= window->scale;
    y *= window->scale;
    x += window->window_x * window->scale;
    y += window->window_y * window->scale;

    return  *(uint32_t*) (window->info.fb_addr + (x * 4) + (y * window->info.fb_pitch));
}


static inline void draw_char_window(psf1_font_t* font, uint32_t x, uint32_t y, char c, uint32_t color, uint32_t bgcolor) {
	char* font_ptr = (char*) font->glyph_buffer + (c * font->header->charsize);

	for (unsigned long i = y; i < y + 16; i++){
		for (unsigned long j = x; j < x + 8; j++){
			if ((*font_ptr & (0b10000000 >> (j - x))) > 0) {
				set_pixel_window(j, i, color);
			} else {
				set_pixel_window(j, i, bgcolor);
            }
		}
		font_ptr++;
	}
}

static inline void draw_string_window(psf1_font_t* font, uint32_t x, uint32_t y, const char* s, uint32_t color, uint32_t bgcolor) {
    int i = 0;
    while (s[i]) {
        draw_char_window(font, x + 8 * i, y, s[i], color, bgcolor);
        i++;
    }
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define RABS(x) (x < 0 ? -x : x)

static inline void draw_line_window(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color) {
	// Bresenham's line algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

	int32_t dx = RABS(x2 - x1);
	int32_t dy = RABS(y2 - y1);
	int32_t sx = x1 < x2 ? 1 : -1;
	int32_t sy = y1 < y2 ? 1 : -1;
	int32_t err = dx - dy;

	while (1) {
		set_pixel_window(x1, y1, color);

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

void window_init(int height, int width, int x, int y, const char* name);

void window_fullscreen();

char window_async_getc();
char window_async_getarrw();
void window_mouse_info(mouse_info_t* info);


void window_optimize();

bool window_redrawn();

void window_open_prog_request(const char* file);

void window_clear(uint32_t color);