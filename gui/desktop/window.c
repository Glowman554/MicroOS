#include <window.h>
#include <types.h>
#include <string.h>
#include <stddef.h>

window_instance_t windows[MAX_WINDOWS];
int window_count = 0;
int focused_window = -1;

void window_add(int x, int y, int width, int height, const char* title, uint32_t bg_color,
                void (*init)(window_instance_t*),
                void (*update)(window_instance_t*, event_t*),
                void (*draw)(window_instance_t*),
                void (*cleanup)(window_instance_t*)) {
    if (window_count >= MAX_WINDOWS) {
        return;
    }
    
    window_instance_t* w = &windows[window_count];
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
    memset(w->title, 0, sizeof(w->title));
    strcpy(w->title, title);
    w->bg_color = bg_color;
    w->title_bar_color = 0x444477;
    w->init = init;
    w->update = update;
    w->draw = draw;
    w->cleanup = cleanup;
    w->is_focused = false;
    w->is_dirty = true;
    
    if (init) {
        init(w);
    }
    
    window_count++;
    focused_window = window_count - 1;
}

void window_close(int idx) {
    if (idx < 0 || idx >= window_count) {
        return;
    }
    
    if (windows[idx].cleanup) {
        windows[idx].cleanup(&windows[idx]);
    }
    
    for (int i = idx; i < window_count - 1; i++) {
        windows[i] = windows[i + 1];
    }
    
    window_count--;
    
    if (focused_window == idx) {
        focused_window = window_count > 0 ? window_count - 1 : -1;
    } else if (focused_window > idx) {
        focused_window--;
    }
}

int window_at_point(int x, int y) {
    if (focused_window >= 0 && focused_window < window_count) {
        window_instance_t* w = &windows[focused_window];
        if (x >= w->x && x < w->x + w->width && y >= w->y && y < w->y + w->height) {
            return focused_window;
        }
    }
    
    for (int i = window_count - 1; i >= 0; i--) {
        if (i == focused_window) {
            continue;
        }
        
        window_instance_t* w = &windows[i];
        if (x >= w->x && x < w->x + w->width && y >= w->y && y < w->y + w->height) {
            return i;
        }
    }
    
    return -1;
}

drag_type_t detect_drag_type(window_instance_t* w, int x, int y) {
    if (y >= w->y && y < w->y + TITLE_BAR_HEIGHT && x >= w->x && x < w->x + w->width) {
        if (x < w->close_button.x) {
            return DRAG_MOVE;
        }
    }
    
    bool on_top = (y >= w->y && y < w->y + RESIZE_BORDER);
    bool on_bottom = (y >= w->y + w->height - RESIZE_BORDER && y < w->y + w->height);
    bool on_left = (x >= w->x && x < w->x + RESIZE_BORDER);
    bool on_right = (x >= w->x + w->width - RESIZE_BORDER && x < w->x + w->width);
    
    if (on_top && on_left)    { 
        return DRAG_RESIZE_NW;
    }
    if (on_top && on_right)   {
        return DRAG_RESIZE_NE;
    }
    if (on_bottom && on_left)  {
        return DRAG_RESIZE_SW;
    }
    if (on_bottom && on_right) {
        return DRAG_RESIZE_SE;
    }
    
    if (on_top)    { 
        return DRAG_RESIZE_N; 
    }
    if (on_bottom) { 
        return DRAG_RESIZE_S; 
    }
    if (on_left)   { 
        return DRAG_RESIZE_W; 
    }
    if (on_right)  { 
        return DRAG_RESIZE_E; 
    }
    
    return DRAG_NONE;
}

void handle_window_drag(window_instance_t* w, int mouse_x, int mouse_y) {
    int dx = mouse_x - w->drag_start_x;
    int dy = mouse_y - w->drag_start_y;
    
    switch (w->drag_state) {
        case DRAG_MOVE:
            w->x = w->drag_start_win_x + dx;
            w->y = w->drag_start_win_y + dy;
            break;
            
        case DRAG_RESIZE_N:
            w->y = w->drag_start_win_y + dy;
            w->height = w->drag_start_win_height - dy;
            if (w->height < 100) {
                w->height = 100;
            }
            break;
            
        case DRAG_RESIZE_S:
            w->height = w->drag_start_win_height + dy;
            if (w->height < 100) {
                w->height = 100;
            }
            break;
            
        case DRAG_RESIZE_W:
            w->x = w->drag_start_win_x + dx;
            w->width = w->drag_start_win_width - dx;
            if (w->width < 150) {
                w->width = 150;
            }
            break;
            
        case DRAG_RESIZE_E:
            w->width = w->drag_start_win_width + dx;
            if (w->width < 150) {
                w->width = 150;
            }
            break;
            
        case DRAG_RESIZE_NE:
            w->y = w->drag_start_win_y + dy;
            w->height = w->drag_start_win_height - dy;
            w->width = w->drag_start_win_width + dx;
            if (w->height < 100) {
                w->height = 100;
            }
            if (w->width < 150) {
                w->width = 150;
            }
            break;
            
        case DRAG_RESIZE_NW:
            w->x = w->drag_start_win_x + dx;
            w->y = w->drag_start_win_y + dy;
            w->width = w->drag_start_win_width - dx;
            w->height = w->drag_start_win_height - dy;
            if (w->height < 100) {
                w->height = 100;
            }
            if (w->width < 150) {
                w->width = 150;
            }
            break;
            
        case DRAG_RESIZE_SE:
            w->width = w->drag_start_win_width + dx;
            w->height = w->drag_start_win_height + dy;
            if (w->height < 100) {
                w->height = 100;
            }
            if (w->width < 150) {
                w->width = 150;
            }
            break;
            
        case DRAG_RESIZE_SW:
            w->x = w->drag_start_win_x + dx;
            w->width = w->drag_start_win_width - dx;
            w->height = w->drag_start_win_height + dy;
            if (w->height < 100) {
                w->height = 100;
            }
            if (w->width < 150) {
                w->width = 150;
            }
            break;
            
        default:
            break;
    }
    
    w->is_dirty = true;
}

int window_get_count(void) {
    return window_count;
}

int window_get_focused(void) {
    return focused_window;
}

void window_set_focused(int idx) {
    if (idx >= -1 && idx < window_count) {
        focused_window = idx;
    }
}

window_instance_t* window_get(int idx) {
    if (idx >= 0 && idx < window_count) {
        return &windows[idx];
    }
    return NULL;
}
