#pragma once

#include <stdint.h>

#ifdef PSF1_DRAW_CHAR_IMPL
#define FB_SET_PX_IMPL
#endif
#include <buildin/framebuffer.h>

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

psf1_font_t psf1_from_buffer(void* buffer);
psf1_font_t psf1_from_file(const char* path);

#ifdef PSF1_DRAW_CHAR_IMPL
#define FB_SET_PX_IMPL
static inline void psf1_draw_char(fb_info_t* info, psf1_font_t* font, uint32_t x, uint32_t y, char c, uint32_t fgcolor, uint32_t bgcolor) {
	char* font_ptr = (char*) font->glyph_buffer + (c * font->header->charsize);

	for (unsigned long i = y; i < y + 16; i++){
		for (unsigned long j = x; j < x + 8; j++){
			if ((*font_ptr & (0b10000000 >> (j - x))) > 0) {
				fb_set_pixel(info, j, i, fgcolor);
			} else {
				fb_set_pixel(info, j, i, bgcolor);
            }
		}
		font_ptr++;
	}
}
#endif