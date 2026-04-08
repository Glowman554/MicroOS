#pragma once

#include <types.h>
#include <graphics.h>

extern psf1_font_t font;

static inline void window_draw_string(window_instance_t* w, int x, int y, const char* str, uint32_t fg_color) {
    desktop_draw_string(&font, w->x + x, w->y + TITLE_BAR_HEIGHT + y, str, fg_color, w->bg_color);
}

static inline void window_draw_char(window_instance_t* w, int x, int y, char c, uint32_t color, uint32_t bgcolor) {
    desktop_draw_char(&font, w->x + x, w->y + y, c, color, bgcolor);
}


static inline void window_draw_line(window_instance_t* w, int x1, int y1, int x2, int y2, uint32_t color) {
    desktop_draw_line(w->x + x1, w->y + TITLE_BAR_HEIGHT + y1, w->x + x2, w->y + TITLE_BAR_HEIGHT + y2, color);
}

static inline void window_draw_pixel(window_instance_t* w, int x, int y, uint32_t color) {
    desktop_set_pixel(w->x + x, w->y + TITLE_BAR_HEIGHT + y, color);
}

static inline void window_fill_rect(window_instance_t* w, int x, int y, int width, int height, uint32_t color) {
    for (int i = x; i < x + width; i++) {
        for (int j = y; j < y + height; j++) {
            desktop_set_pixel(w->x + i, w->y + TITLE_BAR_HEIGHT + j, color);
        }
    }
}

static inline void window_draw_string_bg(window_instance_t* w, int x, int y, const char* str, uint32_t fg_color, uint32_t bg_color) {
    desktop_draw_string(&font, w->x + x, w->y + TITLE_BAR_HEIGHT + y, str, fg_color, bg_color);
}

static inline void window_draw_rect(window_instance_t* w, int x, int y, int width, int height, uint32_t color) {
    desktop_draw_rect(w->x + x, w->y + TITLE_BAR_HEIGHT + y, width, height, color);
}

static inline void window_set_pixel(window_instance_t* w, int x, int y, uint32_t color) {
    desktop_set_pixel(w->x + x, w->y + y, color);
}

static inline void window_draw_fpic(window_instance_t* w, fpic_image_t* pic, int x, int y) {
    desktop_draw_fpic(pic, w->x + x, w->y + y);
}
static inline void window_draw_fpic_scaled(window_instance_t* w, fpic_image_t* pic, int x, int y, int scale) {
    desktop_draw_fpic_scaled(pic, w->x + x, w->y + y, scale);
}
