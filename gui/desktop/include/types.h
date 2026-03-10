#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct window_instance window_instance_t;

#define MAX_WINDOWS 16
#define TITLE_BAR_HEIGHT 20
#define BORDER_WIDTH 4
#define BUTTON_SIZE 16
#define RESIZE_BORDER 8

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charsize;
} psf1_header_t;

typedef struct {
    psf1_header_t* header;
    void* glyph_buffer;
} psf1_font_t;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} click_area_t;

typedef enum {
    EVENT_NONE,
    EVENT_MOUSE_CLICK,
    EVENT_MOUSE_MOVE,
    EVENT_CLOSE,
    EVENT_KEY_PRESS,
    EVENT_ARROW_KEY,
} event_type_t;

#define MOUSE_BUTTON_LEFT   1
#define MOUSE_BUTTON_RIGHT  2
#define MOUSE_BUTTON_MIDDLE 4

#define ARROW_UP    1
#define ARROW_DOWN  2
#define ARROW_LEFT  3
#define ARROW_RIGHT 4

typedef struct {
    event_type_t type;
    int x;
    int y;
    int button;
    char key;
    int arrow;
} event_t;

typedef enum {
    DRAG_NONE,
    DRAG_MOVE,
    DRAG_RESIZE_N,
    DRAG_RESIZE_S,
    DRAG_RESIZE_E,
    DRAG_RESIZE_W,
    DRAG_RESIZE_NE,
    DRAG_RESIZE_NW,
    DRAG_RESIZE_SE,
    DRAG_RESIZE_SW,
} drag_type_t;

typedef struct {
    const char* name;
    void (*register_window)(void);
} window_definition_t;

typedef struct window_instance {
    int x;
    int y;
    int width;
    int height;
    char title[64];
    uint32_t bg_color;
    uint32_t title_bar_color;
    
    void* state;
    
    void (*init)(struct window_instance* w);
    void (*update)(struct window_instance* w, event_t* event);
    void (*draw)(struct window_instance* w);
    void (*cleanup)(struct window_instance* w);
    
    click_area_t close_button;
    bool is_focused;
    bool is_dirty;
    bool is_realtime;
    
    drag_type_t drag_state;
    int drag_start_x;
    int drag_start_y;
    int drag_start_win_x;
    int drag_start_win_y;
    int drag_start_win_width;
    int drag_start_win_height;
} window_instance_t;
