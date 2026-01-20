#pragma once

#include <stdint.h>
#include <stdbool.h>

enum video_mode_e {
	TEXT_80x25,
    CUSTOM
};

typedef struct fb_info {
    uint32_t fb_pitch;
    uint32_t fb_width;
    uint32_t fb_height;
    uint8_t fb_bpp;
    uint32_t fb_addr;
} fb_info_t;

int vmode();
void vpoke(uint32_t offset, uint8_t* val, uint32_t range);
void vpeek(uint32_t offset, uint8_t* val, uint32_t range);
void vcursor(int x, int y);
void vcursor_get(int* x, int* y);
void set_color(char* color, bool background);
void rgb_color(uint32_t color, bool background);