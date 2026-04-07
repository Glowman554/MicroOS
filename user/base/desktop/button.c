#include <button.h>
#include <window_helpers.h>

void button_init(button_t* btn, int x, int y, int width, int height, const char* label, void (*on_click)(window_instance_t* w, void* userdata), void* userdata) {
    btn->x = x;
    btn->y = y;
    btn->width = width;
    btn->height = height;
    btn->label = label;
    btn->bg_color = 0x445566;
    btn->hover_color = 0x5577aa;
    btn->text_color = 0xffffff;
    btn->border_color = 0x666688;
    btn->is_hovered = false;
    btn->on_click = on_click;
    btn->userdata = userdata;
}

void button_draw(button_t* btn, window_instance_t* w) {
    uint32_t color = btn->is_hovered ? btn->hover_color : btn->text_color;
    uint32_t border = btn->is_hovered ? btn->hover_color : btn->border_color;

    window_draw_rect(w, btn->x, btn->y, btn->width, btn->height, btn->bg_color);

    int text_y = btn->y + (btn->height - 16) / 2;
    window_draw_string(w, btn->x + 8, text_y, btn->label, color);
    
    window_draw_line(w, btn->x, btn->y, btn->x + btn->width, btn->y, border);
    window_draw_line(w, btn->x, btn->y + btn->height - 1, btn->x + btn->width, btn->y + btn->height - 1, border);
    window_draw_line(w, btn->x, btn->y, btn->x, btn->y + btn->height, border);
    window_draw_line(w, btn->x + btn->width - 1, btn->y, btn->x + btn->width - 1, btn->y + btn->height, border);
}

bool button_hit_test(button_t* btn, int x, int y) {
    return x >= btn->x && x < btn->x + btn->width &&
           y >= btn->y && y < btn->y + btn->height;
}

bool button_handle_event(button_t* btn, window_instance_t* w, event_t* event) {
    if (event->type == EVENT_MOUSE_MOVE) {
        int rel_x = event->x;
        int rel_y = event->y - TITLE_BAR_HEIGHT;
        bool now_hovered = button_hit_test(btn, rel_x, rel_y);
        if (now_hovered != btn->is_hovered) {
            btn->is_hovered = now_hovered;
            w->is_dirty = true;
        }
        return now_hovered;
    }

    if (event->type == EVENT_MOUSE_CLICK && event->button == MOUSE_BUTTON_LEFT) {
        int rel_x = event->x;
        int rel_y = event->y - TITLE_BAR_HEIGHT;
        if (button_hit_test(btn, rel_x, rel_y)) {
            if (btn->on_click) {
                btn->on_click(w, btn->userdata);
            }
            w->is_dirty = true;
            return true;
        }
    }

    return false;
}
