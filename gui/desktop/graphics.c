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

#define cursor_ns_width 9
#define cursor_ns_height 15
static const uint8_t cursor_resize_ns[] = {
    0,0,0,0,1,0,0,0,0,
    0,0,0,1,2,1,0,0,0,
    0,0,1,2,2,2,1,0,0,
    0,1,2,2,2,2,2,1,0,
    1,1,1,2,2,2,1,1,1,
    0,0,0,2,2,2,0,0,0,
    0,0,0,2,2,2,0,0,0,
    0,0,0,2,2,2,0,0,0,
    0,0,0,2,2,2,0,0,0,
    0,0,0,2,2,2,0,0,0,
    1,1,1,2,2,2,1,1,1,
    0,1,2,2,2,2,2,1,0,
    0,0,1,2,2,2,1,0,0,
    0,0,0,1,2,1,0,0,0,
    0,0,0,0,1,0,0,0,0,
};

#define cursor_ew_width 15
#define cursor_ew_height 9
static const uint8_t cursor_resize_ew[] = {
    0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,
    0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,
    0,0,1,2,1,0,0,0,0,0,1,2,1,0,0,
    0,1,2,2,2,2,2,2,2,2,2,2,2,1,0,
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
    0,1,2,2,2,2,2,2,2,2,2,2,2,1,0,
    0,0,1,2,1,0,0,0,0,0,1,2,1,0,0,
    0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,
    0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,
};

#define cursor_nesw_width 11
#define cursor_nesw_height 11
static const uint8_t cursor_resize_nesw[] = {
    0,0,0,0,0,0,1,1,1,1,1,
    0,0,0,0,0,0,0,1,2,2,1,
    0,0,0,0,0,0,1,2,2,1,0,
    0,0,0,0,0,1,2,2,1,0,0,
    0,0,0,0,1,2,2,1,0,0,0,
    0,0,0,1,2,2,1,0,0,0,0,
    0,0,1,2,2,1,0,0,0,0,0,
    0,1,2,2,1,0,0,0,0,0,0,
    0,1,2,2,1,0,0,0,0,0,0,
    1,2,2,1,0,0,0,0,0,0,0,
    1,1,1,1,1,0,0,0,0,0,0,
};

#define cursor_nwse_width 11
#define cursor_nwse_height 11
static const uint8_t cursor_resize_nwse[] = {
    1,1,1,1,1,0,0,0,0,0,0,
    1,2,2,1,0,0,0,0,0,0,0,
    0,1,2,2,1,0,0,0,0,0,0,
    0,0,1,2,2,1,0,0,0,0,0,
    0,0,0,1,2,2,1,0,0,0,0,
    0,0,0,0,1,2,2,1,0,0,0,
    0,0,0,0,0,1,2,2,1,0,0,
    0,0,0,0,0,0,1,2,2,1,0,
    0,0,0,0,0,0,1,2,2,1,0,
    0,0,0,0,0,0,0,1,2,2,1,
    0,0,0,0,0,0,0,0,1,1,1,
};

void desktop_draw_mouse_pointer(int x, int y, cursor_type_t cursor) {
    const uint8_t* data;
    int w;
    int h;
    int ox = 0;
    int oy = 0;

    switch (cursor) {
        case CURSOR_RESIZE_NS:
            data = cursor_resize_ns;
            w = cursor_ns_width;
            h = cursor_ns_height;
            ox = -w / 2;
            oy = -h / 2;
            break;
        case CURSOR_RESIZE_EW:
            data = cursor_resize_ew;
            w = cursor_ew_width;
            h = cursor_ew_height;
            ox = -w / 2;
            oy = -h / 2;
            break;
        case CURSOR_RESIZE_NESW:
            data = cursor_resize_nesw;
            w = cursor_nesw_width;
            h = cursor_nesw_height;
            ox = -w / 2;
            oy = -h / 2;
            break;
        case CURSOR_RESIZE_NWSE:
            data = cursor_resize_nwse;
            w = cursor_nwse_width;
            h = cursor_nwse_height;
            ox = -w / 2;
            oy = -h / 2;
            break;
        default:
            data = mouse_pointer;
            w = mouse_pointer_width;
            h = mouse_pointer_height;
            ox = 0;
            oy = 0;
            break;
    }

    int current_x = 0;
    int current_y = 0;
    int len = w * h;

    for (int i = 0; i < len; i++) {
        if (data[i] == 2) {
            desktop_set_pixel(x + ox + current_x, y + oy + current_y, 0xFFFFFF);
        } else if (data[i] == 1) {
            desktop_set_pixel(x + ox + current_x, y + oy + current_y, 0x000000);
        }

        current_x++;
        if (current_x >= w) {
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
