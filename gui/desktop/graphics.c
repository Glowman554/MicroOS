#include <graphics.h>
#include <framebuffer.h>

#define RABS(x) (x < 0 ? -x : x)

void desktop_set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= framebuffer.fb_width || y >= framebuffer.fb_height) {
        return;
    }
    *(uint32_t*)(framebuffer.fb_addr + (x * 4) + (y * framebuffer.fb_pitch)) = color;
}

void desktop_draw_char(psf1_font_t* font, uint32_t x, uint32_t y, char c, uint32_t color, uint32_t bgcolor) {
    char* font_ptr = (char*)font->glyph_buffer + (c * font->header->charsize);

    for (unsigned long i = y; i < y + 16; i++) {
        for (unsigned long j = x; j < x + 8; j++) {
            if ((*font_ptr & (0b10000000 >> (j - x))) > 0) {
                desktop_set_pixel(j, i, color);
            } else {
                desktop_set_pixel(j, i, bgcolor);
            }
        }
        font_ptr++;
    }
}

void desktop_draw_string(psf1_font_t* font, uint32_t x, uint32_t y, const char* s, uint32_t color, uint32_t bgcolor) {
    int i = 0;
    while (s[i]) {
        desktop_draw_char(font, x + 8 * i, y, s[i], color, bgcolor);
        i++;
    }
}

void desktop_draw_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color) {
    // Bresenham's line algorithm
    int32_t dx = RABS(x2 - x1);
    int32_t dy = RABS(y2 - y1);
    int32_t sx = x1 < x2 ? 1 : -1;
    int32_t sy = y1 < y2 ? 1 : -1;
    int32_t err = dx - dy;

    while (1) {
        desktop_set_pixel(x1, y1, color);

        if (x1 == x2 && y1 == y2) {
            break;
        }

        int32_t e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

#define mouse_pointer_width 12
#define mouse_pointer_height 19
#define mouse_pointer_len (mouse_pointer_width * mouse_pointer_height)

extern uint8_t mouse_pointer[];


void desktop_draw_mouse_pointer(int x, int y) {
    int current_x = 0;
	int current_y = 0;

    for (int i = 0; i < mouse_pointer_len; i++) {
        if (mouse_pointer[i] == 2) {
            desktop_set_pixel(x + current_x, y + current_y, 0xFFFFFF);
        } else if (mouse_pointer[i] == 1) {
            desktop_set_pixel(x + current_x, y + current_y, 0x000000);
        }

        current_x++;
        if (current_x >= mouse_pointer_width) {
            current_x = 0;
            current_y++;
        }
    }
}

void desktop_draw_rect(int x, int y, int w, int h, uint32_t color) {
    desktop_draw_line(x,     y,     x + w, y,     color);
    desktop_draw_line(x,     y + h, x + w, y + h, color);
    desktop_draw_line(x,     y,     x,     y + h, color);
    desktop_draw_line(x + w, y,     x + w, y + h, color);
}

void desktop_draw_fpic(fpic_image_t* pic, int x, int y) {
    if (!pic || pic->magic != 0xc0ffebabe) {
        return;
    }
    
    for (uint64_t i = 0; i < pic->width; i++) {
        for (uint64_t j = 0; j < pic->height; j++) {
            uint32_t color = pic->pixels[j * pic->width + i];
            uint8_t alpha = (color >> 24) & 0xFF;
            if (alpha > 0) {
                desktop_set_pixel(x + i, y + j, color & 0x00FFFFFF);
            }
        }
    }
}

void desktop_draw_fpic_scaled(fpic_image_t* pic, int x, int y, int scale) {
    if (!pic || pic->magic != 0xc0ffebabe) {
        return;
    }
    
    for (uint64_t i = 0; i < pic->width; i++) {
        for (uint64_t j = 0; j < pic->height; j++) {
            uint32_t color = pic->pixels[j * pic->width + i];
            uint8_t alpha = (color >> 24) & 0xFF;
            if (alpha > 0) {
                for (int si = 0; si < scale; si++) {
                    for (int sj = 0; sj < scale; sj++) {
                        desktop_set_pixel(x + i * scale + si, y + j * scale + sj, color & 0x00FFFFFF);
                    }
                }
            }
        }
    }
}
