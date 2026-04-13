#pragma once

#include <wm_client.h>
#include <wm_protocol.h>
#include <stdint.h>

typedef struct {
    int x, y, w, h;
    const char* label;
    uint32_t bg_color;
    uint32_t hover_color;
    uint32_t text_color;
    uint32_t border_color;
    int is_hovered;
    int was_clicked;
} ui_button_t;

void ui_button_init(ui_button_t* b, int x, int y, int w, int h, const char* label);
int  ui_button_update(ui_button_t* b, wm_event_t* evt);
int  ui_button_clicked(ui_button_t* b);
void ui_button_draw(ui_button_t* b, wm_client_t* c);
