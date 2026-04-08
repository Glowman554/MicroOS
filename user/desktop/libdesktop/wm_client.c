#include <wm_client.h>
#include <wm_protocol.h>

#include <non-standard/sys/mmap.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/file.h>
#include <non-standard/stdio.h>
#include <stdlib.h>
#include <string.h>

#define RABS(x) ((x) < 0 ? -(x) : (x))

wm_psf1_font_t load_wm_font(void) {
    wm_psf1_font_t font;
    font.header = NULL;
    font.glyph_buffer = NULL;

    FILE* f = fopen("dev:font", "r");
    if (!f) {
        return font;
    }

    fsize(f, size);
    void* buf = malloc(size);
    fread(buf, 1, size, f);
    fclose(f);

    font.header = (wm_psf1_header_t*)buf;
    if (font.header->magic[0] != WM_PSF1_MAGIC0 || font.header->magic[1] != WM_PSF1_MAGIC1) {
        free(buf);
        font.header = NULL;
        return font;
    }

    font.glyph_buffer = (void*)((uint8_t*)buf + sizeof(wm_psf1_header_t));
    return font;
}

void wm_client_init(wm_client_t* client) {
    char* slot_env = getenv("WMS");
    if (!slot_env) {
        printf("wm_client_init: WMS env variable not set\n");
        exit(1);
    }

    client->slot = atoi(slot_env);

    void* base = WM_SHM_ADDR(client->slot);

    while (!mmap_mapped(base)) {
        yield();
    }

    client->control = (wm_shared_t*)((uintptr_t)base + WM_CONTROL_OFFSET);
    client->pixels = (uint32_t*)((uintptr_t)base + WM_PIXELS_OFFSET);

    while (client->control->state != WM_STATE_READY) {
        yield();
    }

    client->width = client->control->width;
    client->height = client->control->height;

    client->control->alive = 1;
    client->control->state = WM_STATE_ACK;

    while (client->control->state != WM_STATE_CONNECTED) {
        yield();
    }

    client->font = load_wm_font();

    memset(client->pixels, 0, client->width * client->height * 4);
}

bool wm_client_poll_event(wm_client_t* client, wm_event_t* out) {
    wm_shared_t* ctl = client->control;

    if (ctl->event_read == ctl->event_write) {
        return false;
    }

    *out = ctl->events[ctl->event_read % WM_MAX_EVENTS];
    ctl->event_read++;
    return true;
}

bool wm_client_should_close(wm_client_t* client) {
    return client->control->close_requested != 0;
}

int wm_client_width(wm_client_t* client) {
    return client->control->width;
}

int wm_client_height(wm_client_t* client) {
    return client->control->height;
}

void wm_client_set_pixel(wm_client_t* client, int x, int y, uint32_t color) {
    int w = client->control->width;
    int h = client->control->height;
    if (x < 0 || x >= w || y < 0 || y >= h) {
        return;
    }
    client->pixels[y * w + x] = color;
}

void wm_client_fill_rect(wm_client_t* client, int x, int y, int w, int h, uint32_t color) {
    for (int j = y; j < y + h; j++) {
        for (int i = x; i < x + w; i++) {
            wm_client_set_pixel(client, i, j, color);
        }
    }
}

void wm_client_draw_char(wm_client_t* client, int x, int y, char c, uint32_t fg, uint32_t bg) {
    if (!client->font.header || !client->font.glyph_buffer) {
        return;
    }
    char* font_ptr = (char*)client->font.glyph_buffer + (c * client->font.header->charsize);

    for (int j = 0; j < 16; j++) {
        for (int i = 0; i < 8; i++) {
            if ((*font_ptr & (0b10000000 >> i)) > 0) {
                wm_client_set_pixel(client, x + i, y + j, fg);
            } else {
                wm_client_set_pixel(client, x + i, y + j, bg);
            }
        }
        font_ptr++;
    }
}

void wm_client_draw_string(wm_client_t* client, int x, int y, const char* str, uint32_t fg, uint32_t bg) {
    int i = 0;
    while (str[i]) {
        wm_client_draw_char(client, x + 8 * i, y, str[i], fg, bg);
        i++;
    }
}

void wm_client_draw_line(wm_client_t* client, int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = RABS(x2 - x1);
    int dy = RABS(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        wm_client_set_pixel(client, x1, y1, color);
        if (x1 == x2 && y1 == y2) {
            break;
        }
        int e2 = 2 * err;
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

void wm_client_draw_rect(wm_client_t* client, int x, int y, int w, int h, uint32_t color) {
    wm_client_draw_line(client, x, y, x + w, y, color);
    wm_client_draw_line(client, x, y + h, x + w, y + h, color);
    wm_client_draw_line(client, x, y, x, y + h, color);
    wm_client_draw_line(client, x + w, y, x + w, y + h, color);
}

void wm_client_flush(wm_client_t* client) {
    client->control->dirty = 1;
    while (client->control->dirty) {
        yield();
    }
}

void wm_client_set_title(wm_client_t* client, const char* title) {
    memset(client->control->title, 0, 64);
    size_t len = strnlen(title, 63);
    memcpy(client->control->title, title, len);
}

void wm_client_set_bg_color(wm_client_t* client, uint32_t color) {
    client->control->bg_color = color;
}

void wm_client_set_title_bar_color(wm_client_t* client, uint32_t color) {
    client->control->title_bar_color = color;
}

void wm_client_set_realtime(wm_client_t* client, bool realtime) {
    client->control->is_realtime = realtime ? 1 : 0;
}

// ---------- Button widget ----------

void wm_btn_init(wm_button_t* b, int x, int y, int w, int h, const char* label) {
    b->x = x;
    b->y = y;
    b->w = w;
    b->h = h;
    b->label = label;
    b->bg_color = 0x445566;
    b->hover_color = 0x5577aa;
    b->text_color = 0xffffff;
    b->border_color = 0x666688;
    b->is_hovered = 0;
}

void wm_btn_draw(wm_button_t* b, wm_client_t* c) {
    uint32_t bg = b->is_hovered ? b->hover_color : b->bg_color;

    wm_client_fill_rect(c, b->x, b->y, b->w, b->h, bg);

    int text_y = b->y + (b->h - 16) / 2;
    wm_client_draw_string(c, b->x + 8, text_y, b->label, b->text_color, bg);

    wm_client_draw_line(c, b->x, b->y, b->x + b->w, b->y, b->border_color);
    wm_client_draw_line(c, b->x, b->y + b->h - 1, b->x + b->w, b->y + b->h - 1, b->border_color);
    wm_client_draw_line(c, b->x, b->y, b->x, b->y + b->h, b->border_color);
    wm_client_draw_line(c, b->x + b->w - 1, b->y, b->x + b->w - 1, b->y + b->h, b->border_color);
}

int wm_btn_hit(wm_button_t* b, int x, int y) {
    return x >= b->x && x < b->x + b->w && y >= b->y && y < b->y + b->h;
}

int wm_btn_update_hover(wm_button_t* b, int mouse_x, int mouse_y) {
    int now = wm_btn_hit(b, mouse_x, mouse_y);
    if (now != b->is_hovered) {
        b->is_hovered = now;
        return 1;
    }
    return 0;
}
