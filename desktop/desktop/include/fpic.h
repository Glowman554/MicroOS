#pragma once

#include <stdint.h>

typedef struct {
	uint64_t magic; // 0xc0ffebabe
	uint64_t width;
	uint64_t height;
	uint32_t pixels[];
} __attribute__((packed)) fpic_image_t;

fpic_image_t* load_fpic(const char* file);
uint32_t get_pixel_fpic(fpic_image_t* pic, int x, int y);
void draw_fpic(fpic_image_t* pic, int x, int y);
void draw_fpic_scaled(fpic_image_t* pic, int x, int y, int scale);