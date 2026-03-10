#pragma once

#include <stdint.h>
#include <types.h>
#include <framebuffer.h>

typedef struct {
    uint64_t magic; // 0xc0ffebabe
    uint64_t width;
    uint64_t height;
    uint32_t pixels[];
} __attribute__((packed)) fpic_image_t;

void desktop_set_pixel(uint32_t x, uint32_t y, uint32_t color);
void desktop_draw_char(psf1_font_t* font, uint32_t x, uint32_t y, char c, uint32_t color, uint32_t bgcolor);
void desktop_draw_string(psf1_font_t* font, uint32_t x, uint32_t y, const char* s, uint32_t color, uint32_t bgcolor);
void desktop_draw_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);
void desktop_draw_mouse_pointer(int x, int y, cursor_type_t cursor);
void desktop_draw_rect(int x, int y, int w, int h, uint32_t color);
void desktop_draw_fpic(fpic_image_t* pic, int x, int y);
void desktop_draw_fpic_scaled(fpic_image_t* pic, int x, int y, int scale);
