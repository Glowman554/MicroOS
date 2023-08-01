#pragma once

#include <stdint.h>
#include <stdbool.h>

enum video_mode_e {
	TEXT_80x25,
};

int vmode();
void vpoke(uint32_t offset, uint8_t* val, uint32_t range);
void vpeek(uint32_t offset, uint8_t* val, uint32_t range);
void vcursor(int x, int y);
void vcursor_get(int* x, int* y);
void set_color(char* color, bool background);