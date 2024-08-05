#include <renderer/text_mode_emulation.h>
#include <renderer/text_console.h>
#include <stdint.h>
#include <utils/multiboot.h>
#include <renderer/status_bar.h>
#include <driver/timer_driver.h>

psf1_font_t text_mode_emulation_font = { 0 };

extern char text_console_video[];
// Marcel forced me to do this :(
char text_console_video_old[SCREEN_WIDTH * SCREEN_HEIGHT * 2] = { 0 };

void init_text_mode_emulation(psf1_font_t font) {
    text_mode_emulation_font = font;
}

static inline void set_pixel(void* buffer, uint32_t x, uint32_t y, uint32_t color) {
    *(uint32_t*) (buffer + (x * 4) + (y * 4 * (global_multiboot_info->fb_pitch / 4))) = color;
}

void draw_char(void* buffer, uint32_t x, uint32_t y, char c, uint32_t color, uint32_t bgcolor) {
	char* font_ptr = (char*) text_mode_emulation_font.glyph_buffer + (c * text_mode_emulation_font.header->charsize);

	for (unsigned long i = y; i < y + 16; i++){
		for (unsigned long j = x; j < x + 8; j++){
			if ((*font_ptr & (0b10000000 >> (j - x))) > 0) {
				set_pixel(buffer, j, i, color);
			} else {
				set_pixel(buffer, j, i, bgcolor);
            }
		}
		font_ptr++;
	}
}

uint32_t color_translation_table[] = {
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
};

void text_mode_emulation_update() {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
            int buffer_idx = 2 * (i * SCREEN_WIDTH + j);
            int buffer_idx_color = buffer_idx + 1;

			char chr = text_console_video[buffer_idx];
			char clr = text_console_video[buffer_idx_color];
            if (!chr) {
                chr = ' ';
            }

			char chr_old = text_console_video_old[buffer_idx];
			char clr_old = text_console_video_old[buffer_idx_color];
			
            if (chr != chr_old || clr != clr_old) {
                uint32_t fg = color_translation_table[clr & 0x0f];
                uint32_t bg = color_translation_table[(clr & 0xf0) >> 4];

                draw_char((void*)(uint32_t) global_multiboot_info->fb_addr, j * 8, i * 16, chr, fg, bg);

                text_console_video_old[buffer_idx] = chr;
                text_console_video_old[buffer_idx_color] = clr;
            }
        }
	}

#ifdef STATUS_BAR
    if (!global_timer_driver) {
        draw_status_bar();
    }
#endif
}