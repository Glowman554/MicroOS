#pragma once

#include <stdint.h>

typedef struct {
	uint64_t magic; // 0xc0ffebabe
	uint64_t width;
	uint64_t height;
	uint32_t pixels[];
} __attribute__((packed)) fpic_image_t;


void print_logo();

extern char logo_txt[];
extern int logo_txt_size;

extern fpic_image_t logo_img;