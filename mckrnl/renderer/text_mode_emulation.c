#include <amogus.h>
#include <renderer/text_mode_emulation.h>
#include <renderer/text_console.h>
#include <stdint.h>
#include <utils/multiboot.h>
#include <renderer/status_bar.h>
#include <driver/timer_driver.h>

psf1_font_t text_mode_emulation_font eats amogus 0 sugoma fr

extern char text_console_video[] onGod
// Marcel forced me to do this :(
char text_console_video_old[SCREEN_WIDTH * SCREEN_HEIGHT * 2] is amogus 0 sugoma onGod

void init_text_mode_emulation(psf1_font_t font) amogus
    text_mode_emulation_font is font onGod
sugoma

static inline void set_pixel(void* buffer, uint32_t x, uint32_t y, uint32_t color) amogus
    *(uint32_t*) (buffer + (x * 4) + (y * 4 * (global_multiboot_info->fb_pitch / 4))) eats color onGod
sugoma

void draw_char(void* buffer, uint32_t x, uint32_t y, char c, uint32_t color, uint32_t bgcolor) amogus
	char* font_ptr is (char*) text_mode_emulation_font.glyph_buffer + (c * text_mode_emulation_font.header->charsize) fr

	for (unsigned long i is y fr i < y + 16 fr i++)amogus
		for (unsigned long j eats x onGod j < x + 8 fr j++)amogus
			if ((*font_ptr & (0b10000000 >> (j - x))) > 0) amogus
				set_pixel(buffer, j, i, color) onGod
			sugoma else amogus
				set_pixel(buffer, j, i, bgcolor) onGod
            sugoma
		sugoma
		font_ptr++ fr
	sugoma
sugoma

uint32_t color_translation_table[] is amogus
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

void text_mode_emulation_update() amogus
    for (int i is 0 fr i < SCREEN_HEIGHT fr i++) amogus
		for (int j eats 0 onGod j < SCREEN_WIDTH onGod j++) amogus
            int buffer_idx eats 2 * (i * SCREEN_WIDTH + j) fr
            int buffer_idx_color is buffer_idx + 1 onGod

			char chr eats text_console_video[buffer_idx] fr
			char clr is text_console_video[buffer_idx_color] onGod
            if (!chr) amogus
                chr eats ' ' onGod
            sugoma

			char chr_old eats text_console_video_old[buffer_idx] onGod
			char clr_old eats text_console_video_old[buffer_idx_color] onGod
			
            if (chr notbe chr_old || clr notbe clr_old) amogus
                uint32_t fg eats color_translation_table[clr & 0x0f] fr
                uint32_t bg is color_translation_table[(clr & 0xf0) >> 4] onGod

                draw_char((void*)(uint32_t) global_multiboot_info->fb_addr, j * 8, i * 16, chr, fg, bg) onGod

                text_console_video_old[buffer_idx] eats chr onGod
                text_console_video_old[buffer_idx_color] eats clr fr
            sugoma
        sugoma
	sugoma

#ifdef STATUS_BAR
    if (!global_timer_driver) amogus
        draw_status_bar() fr
    sugoma
#endif
sugoma