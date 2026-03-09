#include "imgview.h"
#include "scanner.h"
#include <window.h>
#include <window_helpers.h>
#include <graphics.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern psf1_font_t font;

typedef struct {
    void* file_buf;
    int file_size;
    format_scanner_t* scanner;
} imgview_state_t;

char s_pending_path[128];

void imgview_init(window_instance_t* w);
void imgview_update(window_instance_t* w, event_t* event);
void imgview_draw(window_instance_t* w);
void imgview_cleanup(window_instance_t* w);

void imgview_open(const char* path) {
    int path_len = strnlen(path, sizeof(s_pending_path) - 1);
    memcpy(s_pending_path, path, path_len);
    s_pending_path[path_len] = '\0';

    const char* base = path;
    for (const char* p = path; *p; p++) {
        if (*p == '/') {
            base = p + 1;
        }
    }
    char title[64] = { 0 };
    memcpy(title, "Image: ", 7);
    int blen = strnlen(base, sizeof(title) - 8);
    memcpy(title + 7, base, blen);

    window_add(80, 80, 400, 300, title, 0x000000, imgview_init, imgview_update, imgview_draw, imgview_cleanup);
}

void imgview_init(window_instance_t* w) {
    imgview_state_t* state = malloc(sizeof(imgview_state_t));
    memset(state, 0, sizeof(imgview_state_t));
    w->state = state;
    w->title_bar_color = 0x222244;
    w->is_dirty = true;

    FILE* f = fopen(s_pending_path, "rb");
    if (!f) {
        return;
    }
    fsize(f, img_file_size);
    void* buf = malloc(img_file_size);
    fread(buf, img_file_size, 1, f);
    fclose(f);

    state->file_buf  = buf;
    state->file_size = (int)img_file_size;
    state->scanner   = get_scanner(buf, (int)img_file_size);
}

void imgview_update(window_instance_t* w, event_t* event) {
    w->is_dirty = true;
}

void imgview_draw(window_instance_t* w) {
    imgview_state_t* state = (imgview_state_t*)w->state;

    for (int x = 0; x < w->width; x++) {
        for (int y = TITLE_BAR_HEIGHT; y < w->height; y++) {
            desktop_set_pixel(w->x + x, w->y + y, 0x000000);
        }
    }

    if (!state || !state->scanner) {
        window_draw_string(w, 4, 4, "Unsupported format", 0xff4444);
        return;
    }

    int img_w, img_h;
    state->scanner->get_size(state->file_buf, state->file_size, &img_w, &img_h);
    if (img_w <= 0 || img_h <= 0) {
        return;
    }

    int content_w = w->width;
    int content_h = w->height - TITLE_BAR_HEIGHT;

    int fit_x = content_w / img_w;
    int fit_y = content_h / img_h;
    int scale = (fit_x < fit_y) ? fit_x : fit_y;
    if (scale < 1) {
        scale = 1;
    }

    int draw_w = img_w * scale;
    int draw_h = img_h * scale;

    int off_x = (content_w - draw_w) / 2;
    int off_y = (content_h - draw_h) / 2;

    for (int px = 0; px < img_w; px++) {
        for (int py = 0; py < img_h; py++) {
            uint32_t color = state->scanner->get_pixel(state->file_buf, state->file_size, px, py);
            uint32_t rgb = color & 0x00FFFFFF;
            for (int sx = 0; sx < scale; sx++) {
                for (int sy = 0; sy < scale; sy++) {
                    desktop_set_pixel(w->x + off_x + px * scale + sx, w->y + TITLE_BAR_HEIGHT + off_y + py * scale + sy, rgb);
                }
            }
        }
    }
}

void imgview_cleanup(window_instance_t* w) {
    imgview_state_t* state = (imgview_state_t*)w->state;
    if (state) {
        if (state->file_buf) {
            free(state->file_buf);
        }
        free(state);
        w->state = NULL;
    }
}
