#pragma once

#include <types.h>

void window_add(int x, int y, int width, int height, const char* title, uint32_t bg_color,
                void (*init)(window_instance_t*),
                void (*update)(window_instance_t*, event_t*),
                void (*draw)(window_instance_t*),
                void (*cleanup)(window_instance_t*));

void window_add_with_state(int x, int y, int width, int height, const char* title, uint32_t bg_color,
                           void* state,
                           void (*init)(window_instance_t*),
                           void (*update)(window_instance_t*, event_t*),
                           void (*draw)(window_instance_t*),
                           void (*cleanup)(window_instance_t*));

void window_close(int idx);
int window_at_point(int x, int y);
drag_type_t detect_drag_type(window_instance_t* w, int x, int y);
void handle_window_drag(window_instance_t* w, int mouse_x, int mouse_y);

int window_get_count(void);
int window_get_focused(void);
void window_set_focused(int idx);
window_instance_t* window_get(int idx);
