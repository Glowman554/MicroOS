#include <non-standart/sys/graphics.h>
#include <non-standart/sys/mouse.h>
#include <non-standart/sys/getc.h>
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
    desktop_start_menu_init();
    
    desktop_draw_all();
    desktop_draw_mouse_pointer(0, 0, CURSOR_DEFAULT);
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
        
        bool should_redraw = true;
        
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

            if (button == MOUSE_BUTTON_LEFT && desktop_start_menu_handle_click(info.x, info.y)) {
                should_redraw = true;
            } else if (button == MOUSE_BUTTON_LEFT && desktop_taskbar_handle_click(info.x, info.y)) {
                should_redraw = true;
            } else {
            int clicked_idx = window_at_point(info.x, info.y);
            
            if (clicked_idx >= 0) {
                window_instance_t* w = window_get(clicked_idx);
                if (w) {
                    if (button == MOUSE_BUTTON_LEFT && check_click_area(&w->close_button, info.x, info.y)) {
                        window_close(clicked_idx);
                        should_redraw = true;
                    } else if (button == MOUSE_BUTTON_LEFT && check_click_area(&w->minimize_button, info.x, info.y)) {
                        w->is_minimized = true;
                        w->is_dirty = true;
                        window_set_focused(-1);
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

        if (info.x != last_mouse_x || info.y != last_mouse_y) {
            int hover_idx = window_at_point(info.x, info.y);
            if (hover_idx >= 0) {
                window_instance_t* hw = window_get(hover_idx);
                if (hw && hw->update) {
                    event_t mevt;
                    mevt.type = EVENT_MOUSE_MOVE;
                    mevt.x = info.x - hw->x;
                    mevt.y = info.y - hw->y;
                    mevt.button = 0;
                    mevt.key = 0;
                    mevt.arrow = 0;
                    hw->update(hw, &mevt);
                    if (hw->is_dirty) {
                        should_redraw = true;
                    }
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
        
        int focused = window_get_focused();
        if (focused >= 0) {
            window_instance_t* fw = window_get(focused);
            if (fw && fw->update) {
                char key = async_getc();
                if (key) {
                    event_t kevt;
                    kevt.type = EVENT_KEY_PRESS;
                    kevt.key = key;
                    kevt.arrow = 0;
                    kevt.x = 0;
                    kevt.y = 0;
                    kevt.button = 0;
                    fw->update(fw, &kevt);
                    if (fw->is_dirty) should_redraw = true;
                } else {
                    int arrow = async_getarrw();
                    if (arrow) {
                        event_t aevt;
                        aevt.type = EVENT_ARROW_KEY;
                        aevt.key = 0;
                        aevt.arrow = arrow;
                        aevt.x = 0;
                        aevt.y = 0;
                        aevt.button = 0;
                        fw->update(fw, &aevt);
                        if (fw->is_dirty) should_redraw = true;
                    }
                }
            }
        }

        for (int i = 0; i < window_get_count(); i++) {
            window_instance_t* w = window_get(i);
            if (w && w->is_realtime && w->update && !w->is_minimized) {
                event_t none_evt;
                none_evt.type = EVENT_NONE;
                none_evt.key = 0;
                none_evt.arrow = 0;
                none_evt.x = 0;
                none_evt.y = 0;
                none_evt.button = 0;
                w->update(w, &none_evt);
                if (w->is_dirty) {
                    should_redraw = true;
                }
            }
        }
        
        if (should_redraw) {
            cursor_type_t cursor = CURSOR_DEFAULT;

            if (dragging_window >= 0) {
                window_instance_t* dw = window_get(dragging_window);
                if (dw) {
                    switch (dw->drag_state) {
                        case DRAG_RESIZE_N:
                        case DRAG_RESIZE_S:
                            cursor = CURSOR_RESIZE_NS;
                            break;
                        case DRAG_RESIZE_E:
                        case DRAG_RESIZE_W:
                            cursor = CURSOR_RESIZE_EW;
                            break;
                        case DRAG_RESIZE_NE:
                        case DRAG_RESIZE_SW:
                            cursor = CURSOR_RESIZE_NESW;
                            break;
                        case DRAG_RESIZE_NW:
                        case DRAG_RESIZE_SE:
                            cursor = CURSOR_RESIZE_NWSE;
                            break;
                        default:
                            break;
                    }
                }
            } else {
                int hover_win = window_at_point(info.x, info.y);
                if (hover_win >= 0) {
                    window_instance_t* hw = window_get(hover_win);
                    if (hw) {
                        drag_type_t potential = detect_drag_type(hw, info.x, info.y);
                        switch (potential) {
                            case DRAG_RESIZE_N:
                            case DRAG_RESIZE_S:
                                cursor = CURSOR_RESIZE_NS;
                                break;
                            case DRAG_RESIZE_E:
                            case DRAG_RESIZE_W:
                                cursor = CURSOR_RESIZE_EW;
                                break;
                            case DRAG_RESIZE_NE:
                            case DRAG_RESIZE_SW:
                                cursor = CURSOR_RESIZE_NESW;
                                break;
                            case DRAG_RESIZE_NW:
                            case DRAG_RESIZE_SE:
                                cursor = CURSOR_RESIZE_NWSE;
                                break;
                            default:
                                break;
                        }
                    }
                }
            }

            desktop_draw_all();
            desktop_draw_mouse_pointer(info.x, info.y, cursor);
            fb_flush();
        }
    }
    
    return 0;
}
