#include <window/vconsole.h>
#include <window.h>

int vc_cursor_x = 0;
int vc_cursor_y = 0;

uint32_t vc_bgcolor = 0x0;
uint32_t vc_color = 0xffffffff;

psf1_font_t vc_font;

void vconsole_init(psf1_font_t* font) {
    vc_font = *font;
    vconsole_clear();
}

void vconsole_clear() {
    vc_cursor_x = 0;
    vc_cursor_y = 0;
    window_clear(vc_bgcolor);
}


void vconsole_putc(char c) {
    if(c == 0 || vconsole_ansi_process(c)) {
		return;
	}

	if(c == '\b') {
		draw_char_window(&vc_font, vc_cursor_x, vc_cursor_y, ' ', vc_color, vc_bgcolor);

		if (vc_cursor_x - 16 >= 0) {			
			vc_cursor_x -= 8;
		}
		return;
	}

	if(vc_cursor_x + 16 > window->window_width || c == '\n') {
		vc_cursor_x = 0;
		vc_cursor_y += 16;
	} else {
		vc_cursor_x += 8;
	}

	if (vc_cursor_y + 16 > window->window_height) {
        for (int y = 16; y <= window->window_height - 1; y++) {
            for (int x = 0; x < window->window_width; x++) {
                set_pixel_window(x, y - 16, get_pixel_window(x, y));
            }
        }

        for (int y = window->window_height - 16; y <= window->window_height - 1; y++) {
            for (int x = 0; x < window->window_width; x++) {
                set_pixel_window(x, y, vc_bgcolor);
            }
        }
		vc_cursor_y -= 16;
	}

	if(c == '\n') {
		return;
	}

	if (c >= 20 && c <= 126) {
		draw_char_window(&vc_font, vc_cursor_x, vc_cursor_y, c, vc_color, vc_bgcolor);
	}
}

void vconsole_puts(const char* s) {
    while (*s) {
        vconsole_putc(*s++);
    }
}

void vconsole_set_color(uint32_t color, bool background) {
	if (background) {
		vc_bgcolor = color;
	} else {
		vc_color = color;
	}
}