#include <sys/graphics.h>
#include <sys/mouse.h>
#include <stdbool.h>

#include <types.h>
#include <framebuffer.h>
#include <graphics.h>
#include <font.h>
#include <window.h>
#include <desktop.h>
#include <windows.h>

#include "windows/launcher/launcher.h"

psf1_font_t font;

static inline bool check_click_area(click_area_t* area, int x, int y) {
    return x >= area->x && x < area->x + area->width &&
           y >= area->y && y < area->y + area->height;
}

int main() {
    fb_init();
    
    font = load_psf1_font("dev:font");

    register_windows();

    register_launcher_window();
    
    desktop_draw_all();
    desktop_draw_mouse_pointer(0, 0);
    fb_flush();
    
    int last_mouse_x = 0;
    int last_mouse_y = 0;
    bool last_button_left = false;
    bool last_button_right = false;
    bool last_button_middle = false;
    int dragging_window = -1;
    
    while (true) {
        mouse_info_t info;
        mouse_info(&info);
        
        bool should_redraw = false;
        
        if ((info.button_left && !last_button_left) ||
            (info.button_right && !last_button_right) ||
            (info.button_middle && !last_button_middle)) {
            int button = 0;
            if (info.button_left && !last_button_left) {
                button = MOUSE_BUTTON_LEFT;
            } else if (info.button_right && !last_button_right) {
                button = MOUSE_BUTTON_RIGHT;
            } else if (info.button_middle && !last_button_middle) {
                button = MOUSE_BUTTON_MIDDLE;
            }
            
            int clicked_idx = window_at_point(info.x, info.y);
            
            if (clicked_idx >= 0) {
                window_instance_t* w = window_get(clicked_idx);
                if (w) {
                    if (button == MOUSE_BUTTON_LEFT && check_click_area(&w->close_button, info.x, info.y)) {
                        window_close(clicked_idx);
                        should_redraw = true;
                    } else {
                        if (clicked_idx != window_get_focused()) {
                            window_set_focused(clicked_idx);
                            should_redraw = true;
                        }
                        
                        drag_type_t drag = DRAG_NONE;
                        if (button == MOUSE_BUTTON_LEFT) {
                            drag = detect_drag_type(w, info.x, info.y);
                        }
                        
                        if (drag != DRAG_NONE) {
                            dragging_window = clicked_idx;
                            w->drag_state = drag;
                            w->drag_start_x = info.x;
                            w->drag_start_y = info.y;
                            w->drag_start_win_x = w->x;
                            w->drag_start_win_y = w->y;
                            w->drag_start_win_width = w->width;
                            w->drag_start_win_height = w->height;
                        } else {
                            event_t event;
                            event.type = EVENT_MOUSE_CLICK;
                            event.x = info.x - w->x;
                            event.y = info.y - w->y;
                            event.button = button;
                            
                            if (w->update) {
                                w->update(w, &event);
                            }
                            
                            if (w->is_dirty) {
                                should_redraw = true;
                            }
                        }
                    }
                }
            }
        }
        
        if (info.button_left && dragging_window >= 0) {
            window_instance_t* w = window_get(dragging_window);
            if (w) {
                if (info.x != last_mouse_x || info.y != last_mouse_y) {
                    handle_window_drag(w, info.x, info.y);
                    should_redraw = true;
                }
            }
        }
        
        if (!info.button_left && last_button_left) {
            if (dragging_window >= 0) {
                window_instance_t* w = window_get(dragging_window);
                if (w) {
                    w->drag_state = DRAG_NONE;
                }
                dragging_window = -1;
            }
        }
        
        last_mouse_x = info.x;
        last_mouse_y = info.y;
        last_button_left = info.button_left;
        last_button_right = info.button_right;
        last_button_middle = info.button_middle;
        
        should_redraw = true;
        
        if (should_redraw) {
            desktop_draw_all();
            desktop_draw_mouse_pointer(info.x, info.y);
            fb_flush();
        }
    }
    
    return 0;
}
