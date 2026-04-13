#pragma once

#include <wm_protocol.h>
#include <stdint.h>
#include <stdbool.h>

#define WM_PSF1_MAGIC0 0x36
#define WM_PSF1_MAGIC1 0x04

typedef struct {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charsize;
} wm_psf1_header_t;

typedef struct {
    wm_psf1_header_t* header;
    void* glyph_buffer;
} wm_psf1_font_t;

// Client context
typedef struct {
    wm_shared_t* control;
    uint32_t* pixels;
    wm_psf1_font_t font;
    int width;
    int height;
    int slot;
} wm_client_t;

void wm_client_init(wm_client_t* client);

bool wm_client_poll_event(wm_client_t* client, wm_event_t* out);

bool wm_client_should_close(wm_client_t* client);

int wm_client_width(wm_client_t* client);
int wm_client_height(wm_client_t* client);

void wm_client_set_pixel(wm_client_t* client, int x, int y, uint32_t color);
void wm_client_fill_rect(wm_client_t* client, int x, int y, int w, int h, uint32_t color);
void wm_client_draw_string(wm_client_t* client, int x, int y, const char* str, uint32_t fg, uint32_t bg);
void wm_client_draw_char(wm_client_t* client, int x, int y, char c, uint32_t fg, uint32_t bg);
void wm_client_draw_line(wm_client_t* client, int x1, int y1, int x2, int y2, uint32_t color);
void wm_client_draw_rect(wm_client_t* client, int x, int y, int w, int h, uint32_t color);

void wm_client_flush(wm_client_t* client);

void wm_client_set_title(wm_client_t* client, const char* title);
void wm_client_set_bg_color(wm_client_t* client, uint32_t color);
void wm_client_set_title_bar_color(wm_client_t* client, uint32_t color);

