#pragma once

#include <types.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int x;
    int y;
    int width;
    int height;
    const char* label;
    uint32_t bg_color;
    uint32_t hover_color;
    uint32_t text_color;
    uint32_t border_color;
    bool is_hovered;
    void (*on_click)(window_instance_t* w, void* userdata);
    void* userdata;
} button_t;

void button_init(button_t* btn, int x, int y, int width, int height, const char* label, void (*on_click)(window_instance_t* w, void* userdata), void* userdata);
void button_draw(button_t* btn, window_instance_t* w);
bool button_hit_test(button_t* btn, int x, int y);
bool button_handle_event(button_t* btn, window_instance_t* w, event_t* event);
