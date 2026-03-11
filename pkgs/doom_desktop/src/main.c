#include "DOOM/DOOM.h"
#include "DOOM/doomdef.h"
#define DOOM_IMPLEMENTATION

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <types.h>
#include <window.h>
#include <window_helpers.h>
#include <windows.h>

#define DOOM_W 320
#define DOOM_H 200
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
    int tick;
    int next_key_up;
    bool initialized;
} doom_state_t;

doom_state_t* s_doom_state = NULL;

void doom_gettime_cb(int* sec, int* usec) {
    int tick = s_doom_state ? s_doom_state->tick : 0;
    *sec = tick / TICRATE;
    *usec = (tick % TICRATE) * (1000000 / TICRATE);
}

void doom_win_init(window_instance_t* w) {
    doom_state_t* st = malloc(sizeof(doom_state_t));
    memset(st, 0, sizeof(doom_state_t));
    w->state = st;
    w->title_bar_color = 0x880000;
    w->is_dirty = true;
    w->is_realtime = true;

    s_doom_state = st;

    doom_set_gettime(doom_gettime_cb);

    doom_set_default_int("key_up", DOOM_KEY_W);
    doom_set_default_int("key_down", DOOM_KEY_S);
    doom_set_default_int("key_strafeleft", DOOM_KEY_A);
    doom_set_default_int("key_straferight", DOOM_KEY_D);
    doom_set_default_int("key_use", DOOM_KEY_E);
    doom_set_default_int("key_fire", 'f');
    doom_set_default_int("mouse_move", 0);

    char* argv[] = { "doom", NULL };
    doom_init(1, argv, 0);
    st->initialized = true;
}

void doom_win_update(window_instance_t* w, event_t* event) {
    doom_state_t* st = (doom_state_t*)w->state;
    if (!st || !st->initialized) {
        return;
    }

    s_doom_state = st;

    if (st->next_key_up) {
        doom_key_up(st->next_key_up);
        st->next_key_up = 0;
    }

    if (event->type == EVENT_KEY_PRESS && event->key) {
        doom_key_down(event->key);
        st->next_key_up = event->key;
    } else if (event->type == EVENT_ARROW_KEY && event->arrow) {
        doom_key_t key = DOOM_KEY_UNKNOWN;
        switch (event->arrow) {
            case 1: key = DOOM_KEY_UP_ARROW; break;
            case 2: key = DOOM_KEY_DOWN_ARROW; break;
            case 3: key = DOOM_KEY_LEFT_ARROW; break;
            case 4: key = DOOM_KEY_RIGHT_ARROW; break;
        }
        if (key != DOOM_KEY_UNKNOWN) {
            doom_key_down(key);
            st->next_key_up = key;
        }
    }

    doom_update();
    st->tick++;
    w->is_dirty = true;
}

void doom_win_draw(window_instance_t* w) {
    doom_state_t* st = (doom_state_t*)w->state;

    int content_w = w->width;
    int content_h = w->height - TITLE_BAR_HEIGHT;

    for (int x = 0; x < w->width; x++) {
        for (int y = TITLE_BAR_HEIGHT; y < w->height; y++) {
            window_set_pixel(w, x, y, 0x000000);
        }
    }

    if (!st || !st->initialized) {
        return;
    }

    const uint32_t* fb = (const uint32_t*)doom_get_framebuffer(4);
    if (!fb) {
        return;
    }

    int scale = MIN(content_w / DOOM_W, content_h / DOOM_H);
    if (scale < 1) {
        scale = 1;
    }

    int draw_w = DOOM_W * scale;
    int draw_h = DOOM_H * scale;
    int off_x = (content_w - draw_w) / 2;
    int off_y = (content_h - draw_h) / 2;

    for (int py = 0; py < DOOM_H; py++) {
        for (int px = 0; px < DOOM_W; px++) {
            uint32_t src = fb[px + py * DOOM_W]; /* RGBA (R low, A high) */
            /* convert to 0x00RRGGBB */
            uint32_t r = (src >>  0) & 0xFF;
            uint32_t g = (src >>  8) & 0xFF;
            uint32_t b = (src >> 16) & 0xFF;
            uint32_t rgb = (r << 16) | (g << 8) | b;

            for (int sy = 0; sy < scale; sy++) {
                for (int sx = 0; sx < scale; sx++) {
                    window_set_pixel(w, off_x + px * scale + sx, TITLE_BAR_HEIGHT + off_y + py * scale + sy, rgb);
                }
            }
        }
    }
}

void doom_win_cleanup(window_instance_t* w) {
    doom_state_t* st = (doom_state_t*)w->state;
    if (st) {
        free(st);
        w->state = NULL;
    }
}

void register_doom_window(void) {
    window_add(50, 50, 640, 400 + TITLE_BAR_HEIGHT, "DOOM", 0x000000, doom_win_init, doom_win_update, doom_win_draw, doom_win_cleanup);
}

window_definition_t doom_definition = {
    .name = "DOOM",
    .register_window = register_doom_window,
};

void init(void) {
    register_window(&doom_definition);
}