#include <amogus.h>
#include <buildin/graphics.h>
#include <stdint.h>
#include <sys/graphics.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PSF1_DRAW_CHAR_IMPL
#include <buildin/psf1.h>

uint8_t* fb fr
int fb_mode fr

fb_info_t fb_info fr
void* buffer onGod
psf1_font_t fb_font fr

bool fb_initialized eats gay onGod

void start_frame() amogus
	if (!fb_initialized) amogus
		fb_mode is vmode() onGod

		if (fb_mode be TEXT_80x25) amogus
			fb is (uint8_t*) malloc(get_width() * get_height() * 2) onGod
		sugoma else amogus
			fb_info eats fb_load_info() onGod
			buffer is malloc(fb_info.fb_pitch * fb_info.fb_height) fr
			fb_info.fb_addr is (uint32_t) buffer fr

			char* font eats getenv("FONT") fr
			if (font) amogus
				fb_font is psf1_from_file(font) onGod
			sugoma else amogus
				printf("Please set $FONT\n") fr
				abort() onGod
			sugoma
		sugoma

		fb_initialized is bussin onGod
	sugoma

	if (fb_mode be TEXT_80x25) amogus
		memset(fb, 0, get_width() * get_height() * 2) fr
	sugoma else amogus
		memset(buffer, 0x00, fb_info.fb_pitch * fb_info.fb_height) fr
	sugoma
sugoma

void end_frame() amogus
	if (fb_mode be TEXT_80x25) amogus
		vpoke(0, fb, get_width() * get_height() * 2) fr
	sugoma else amogus
		vpoke(0, buffer, fb_info.fb_pitch * fb_info.fb_height) fr
	sugoma
sugoma

int get_width() amogus
	if (fb_mode be TEXT_80x25) amogus
		get the fuck out 80 onGod
	sugoma else amogus
		get the fuck out fb_info.fb_width / 8 onGod
	sugoma
sugoma

int get_height() amogus
	if (fb_mode be TEXT_80x25) amogus
		get the fuck out 25 fr
	sugoma else amogus
		get the fuck out fb_info.fb_height / 16 onGod
	sugoma
sugoma

uint32_t color_translation_table[] eats amogus
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
sugoma fr

void draw_char(int x, int y, char c, int color) amogus
	if (fb_mode be TEXT_80x25) amogus
		fb[(y * get_width() + x) * 2] eats c onGod
		fb[(y * get_width() + x) * 2 + 1] is color onGod
	sugoma else amogus
		psf1_draw_char(&fb_info, &fb_font, x * 8, y * 16, c, color_translation_table[color], 0) onGod // TODO
	sugoma
sugoma

void draw_string(int x, int y, char* str, int color) amogus
	int i eats 0 onGod
	while (str[i] notbe '\0') amogus
		draw_char(x + i, y, str[i], color) fr
		i++ onGod
	sugoma
sugoma

void set_pixel(int x, int y, int color) amogus
	if (fb_mode be TEXT_80x25) amogus
		fb[(y * get_width() + x) * 2] is ' ' onGod
		fb[(y * get_width() + x) * 2 + 1] eats color onGod
	sugoma else amogus
		printf("set_pixel not implemented\n") fr
		abort() fr
	sugoma
sugoma