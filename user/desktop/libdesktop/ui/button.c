#include <ui/button.h>

void ui_button_init(ui_button_t* b, int x, int y, int w, int h, const char* label) {
    b->x = x;
    b->y = y;
    b->w = w;
    b->h = h;
    b->label = label;
    b->bg_color = 0x445566;
    b->hover_color = 0x5577aa;
    b->text_color = 0xffffff;
    b->border_color = 0x666688;
    b->is_hovered = 0;
    b->was_clicked = 0;
}

int hit_test(ui_button_t* b, int x, int y) {
    return x >= b->x && x < b->x + b->w && y >= b->y && y < b->y + b->h;
}

int ui_button_update(ui_button_t* b, wm_event_t* evt) {
    int dirty = 0;

    if (evt->type == WM_EVENT_MOUSE_MOVE) {
        int now = hit_test(b, evt->x, evt->y);
        if (now != b->is_hovered) {
            b->is_hovered = now;
            dirty = 1;
        }
    }

    if (evt->type == WM_EVENT_MOUSE_CLICK && evt->button == WM_MOUSE_BUTTON_LEFT) {
        if (hit_test(b, evt->x, evt->y)) {
            b->was_clicked = 1;
            dirty = 1;
        }
    }

    return dirty;
}

int ui_button_clicked(ui_button_t* b) {
    if (b->was_clicked) {
        b->was_clicked = 0;
        return 1;
    }
    return 0;
}

void ui_button_draw(ui_button_t* b, wm_client_t* c) {
    uint32_t bg = b->is_hovered ? b->hover_color : b->bg_color;

    wm_client_fill_rect(c, b->x, b->y, b->w, b->h, bg);

    int text_y = b->y + (b->h - 16) / 2;
    wm_client_draw_string(c, b->x + 8, text_y, b->label, b->text_color, bg);

    wm_client_draw_line(c, b->x, b->y, b->x + b->w, b->y, b->border_color);
    wm_client_draw_line(c, b->x, b->y + b->h - 1, b->x + b->w, b->y + b->h - 1, b->border_color);
    wm_client_draw_line(c, b->x, b->y, b->x, b->y + b->h, b->border_color);
    wm_client_draw_line(c, b->x + b->w - 1, b->y, b->x + b->w - 1, b->y + b->h, b->border_color);
}
