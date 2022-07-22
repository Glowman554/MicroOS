#pragma once

#include <stdint.h>

enum video_mode_e {
	TEXT_80x25,
};

int vmode();
void vpoke(uint32_t offset, uint8_t val);