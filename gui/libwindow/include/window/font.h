#pragma once

#include <stdint.h>

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct {
	uint8_t magic[2];
	uint8_t mode;
	uint8_t charsize;
} psf1_header_t;

typedef struct {
	psf1_header_t* header;
	void* glyph_buffer;
} psf1_font_t;

psf1_font_t psf1_buffer_to_font(void* buffer);
psf1_font_t load_psf1_font(const char* path);