#pragma once

#include <stdint.h>
#include <stdbool.h>

#define WM_SHM_BASE         0xA0000000
#define WM_SHM_SLOT_SIZE    0x200000 
#define WM_CONTROL_OFFSET   0x0000
#define WM_PIXELS_OFFSET    0x1000
#define WM_PAGE_SIZE        0x1000

#define WM_MAX_EVENTS       32
#define WM_MAX_PIXEL_WIDTH  800
#define WM_MAX_PIXEL_HEIGHT 600
#define WM_MAX_PIXEL_BYTES  (WM_MAX_PIXEL_WIDTH * WM_MAX_PIXEL_HEIGHT * 4)
#define WM_MAX_PIXEL_PAGES  ((WM_MAX_PIXEL_BYTES + WM_PAGE_SIZE - 1) / WM_PAGE_SIZE)
#define WM_MAX_SLOTS        16

#define WM_SHM_ADDR(slot)       ((void*)(WM_SHM_BASE + (slot) * WM_SHM_SLOT_SIZE))
#define WM_SHM_CONTROL(slot)    ((wm_shared_t*)((uintptr_t)WM_SHM_ADDR(slot) + WM_CONTROL_OFFSET))
#define WM_SHM_PIXELS(slot)     ((uint32_t*)((uintptr_t)WM_SHM_ADDR(slot) + WM_PIXELS_OFFSET))

enum {
    WM_STATE_INIT = 0,
    WM_STATE_READY = 1,
    WM_STATE_ACK = 2,
    WM_STATE_CONNECTED = 3
};

#define WM_EVENT_NONE           0
#define WM_EVENT_MOUSE_CLICK    1
#define WM_EVENT_MOUSE_MOVE     2
#define WM_EVENT_CLOSE          3
#define WM_EVENT_KEY_PRESS      4
#define WM_EVENT_ARROW_KEY      5

#define WM_MOUSE_BUTTON_LEFT    1
#define WM_MOUSE_BUTTON_RIGHT   2
#define WM_MOUSE_BUTTON_MIDDLE  4

#define WM_ARROW_UP     1
#define WM_ARROW_DOWN   2
#define WM_ARROW_LEFT   3
#define WM_ARROW_RIGHT  4

typedef struct {
    int type;
    int x;
    int y;
    int button;
    char key;
    int arrow;
} wm_event_t;

typedef struct {
    volatile int state;

    int width;
    int height;

    volatile int event_write;
    volatile int event_read;
    wm_event_t events[WM_MAX_EVENTS];

    volatile int dirty;
    volatile int alive;
    volatile int close_requested;
    char title[64];
    uint32_t bg_color;
    uint32_t title_bar_color;
} wm_shared_t;
