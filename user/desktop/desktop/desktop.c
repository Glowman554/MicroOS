#include <desktop.h>
#include <graphics.h>
#include <window.h>
#include <windows.h>
#include <external_window.h>
#include <types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <non-standard/stdio.h>
#include <non-standard/sys/time.h>
#include <non-standard/sys/message.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/mmap.h>
#include <non-standard/buildin/unix_time.h>
#include <non-standard/buildin/data/array.h>


#define MAX_ASSOCS 16

typedef struct {
    char ext[16];
    window_definition_t* def;
    void (*open_fn)(window_definition_t* def, const char* path);
} file_assoc_t;

file_assoc_t s_assocs[MAX_ASSOCS];
int s_assoc_count = 0;

void edit_open_ext(const char* path);

void desktop_register_file_assoc(const char* ext, window_definition_t* def, void (*open_fn)(window_definition_t* def, const char* path)) {
    if (s_assoc_count >= MAX_ASSOCS) {
        return;
    }
    int ext_len = strnlen(ext, sizeof(s_assocs[0].ext) - 1);
    memcpy(s_assocs[s_assoc_count].ext, ext, ext_len);
    s_assocs[s_assoc_count].ext[ext_len] = '\0';
    s_assocs[s_assoc_count].def = def;
    s_assocs[s_assoc_count].open_fn = open_fn;
    s_assoc_count++;
}

void desktop_open_file(const char* path) {
    const char* dot = NULL;
    for (const char* p = path; *p; p++) {
        if (*p == '.') {
            dot = p;
        }
    }
    if (!dot) {
        return;
    }
    const char* ext = dot + 1;
    for (int i = 0; i < s_assoc_count; i++) {
        if (strcmp(s_assocs[i].ext, ext) == 0) {
            s_assocs[i].open_fn(s_assocs[i].def, path);
            return;
        }
    }
}

void desktop_draw_background(void) {
    for (int x = 0; x < framebuffer.fb_width; x++) {
        for (int y = 0; y < framebuffer.fb_height; y++) {
            desktop_set_pixel(x, y, 0x2a2a3e);
            // desktop_set_pixel(x, y, 0x1a1a2e);
        }
    }
}

extern psf1_font_t font;

void desktop_draw_window_chrome(window_instance_t* w) {
    for (int i = w->x; i < w->x + w->width; i++) {
        for (int j = w->y; j < w->y + TITLE_BAR_HEIGHT; j++) {
            desktop_set_pixel(i, j, w->title_bar_color);
        }
    }
    
    desktop_draw_string(&font, w->x + 4, w->y + 2, w->title, 0xffffff, w->title_bar_color);
    
    w->close_button.x = w->x + w->width - BUTTON_SIZE - 2;
    w->close_button.y = w->y + 2;
    w->close_button.width = BUTTON_SIZE;
    w->close_button.height = BUTTON_SIZE;
    
    desktop_draw_line(w->close_button.x, w->close_button.y, w->close_button.x + BUTTON_SIZE, w->close_button.y, 0xff0000);
    desktop_draw_line(w->close_button.x, w->close_button.y, w->close_button.x, w->close_button.y + BUTTON_SIZE, 0xff0000);
    desktop_draw_line(w->close_button.x + BUTTON_SIZE, w->close_button.y, w->close_button.x + BUTTON_SIZE, w->close_button.y + BUTTON_SIZE, 0xff0000);
    desktop_draw_line(w->close_button.x, w->close_button.y + BUTTON_SIZE, w->close_button.x + BUTTON_SIZE, w->close_button.y + BUTTON_SIZE, 0xff0000);
    
    w->minimize_button.x = w->close_button.x - BUTTON_SIZE - 4;
    w->minimize_button.y = w->y + 2;
    w->minimize_button.width = BUTTON_SIZE;
    w->minimize_button.height = BUTTON_SIZE;
    
    desktop_draw_line(w->minimize_button.x, w->minimize_button.y, w->minimize_button.x + BUTTON_SIZE, w->minimize_button.y, 0xffcc00);
    desktop_draw_line(w->minimize_button.x, w->minimize_button.y, w->minimize_button.x, w->minimize_button.y + BUTTON_SIZE, 0xffcc00);
    desktop_draw_line(w->minimize_button.x + BUTTON_SIZE, w->minimize_button.y, w->minimize_button.x + BUTTON_SIZE, w->minimize_button.y + BUTTON_SIZE, 0xffcc00);
    desktop_draw_line(w->minimize_button.x, w->minimize_button.y + BUTTON_SIZE, w->minimize_button.x + BUTTON_SIZE, w->minimize_button.y + BUTTON_SIZE, 0xffcc00);
    desktop_draw_line(w->minimize_button.x + 3, w->minimize_button.y + BUTTON_SIZE - 3, w->minimize_button.x + BUTTON_SIZE - 3, w->minimize_button.y + BUTTON_SIZE - 3, 0xffffff);
    
    for (int i = w->x; i < w->x + w->width; i++) {
        for (int j = w->y + TITLE_BAR_HEIGHT; j < w->y + w->height; j++) {
            desktop_set_pixel(i, j, w->bg_color);
        }
    }    
}

#define TASKBAR_HEIGHT 24
#define TASKBAR_PADDING 4
#define TASKBAR_ITEM_MIN_WIDTH 80

#define START_BUTTON_WIDTH 56
#define START_MENU_ITEM_H 24
#define START_MENU_WIDTH 180
#define START_MENU_PADDING 4

bool s_start_menu_open = false;
click_area_t s_start_button = { 0 };

#define START_MENU_MAX_ITEMS 32
click_area_t s_menu_items[START_MENU_MAX_ITEMS];
int s_menu_item_count = 0;

click_area_t s_taskbar_items[MAX_WINDOWS];
int s_taskbar_item_window_idx[MAX_WINDOWS];
int s_taskbar_item_count = 0;

void desktop_start_menu_init(void) {
    s_start_menu_open = false;
}

bool desktop_is_start_menu_open(void) {
    return s_start_menu_open;
}

void desktop_toggle_start_menu(void) {
    s_start_menu_open = !s_start_menu_open;
}

bool desktop_start_menu_handle_click(int x, int y) {
    if (x >= s_start_button.x && x < s_start_button.x + s_start_button.width &&
        y >= s_start_button.y && y < s_start_button.y + s_start_button.height) {
        desktop_toggle_start_menu();
        return true;
    }

    if (s_start_menu_open) {
        for (int i = 0; i < s_menu_item_count; i++) {
            click_area_t* item = &s_menu_items[i];
            if (x >= item->x && x < item->x + item->width && y >= item->y && y < item->y + item->height) {
                window_definition_t* def = window_definitions[i];
                def->register_window(def);
                s_start_menu_open = false;
                return true;
            }
        }
        s_start_menu_open = false;
        return true;
    }

    return false;
}

void desktop_draw_start_menu(void) {
    int count = array_length(window_definitions);
    if (count > START_MENU_MAX_ITEMS) {
        count = START_MENU_MAX_ITEMS;
    }
    s_menu_item_count = count;

    int menu_h = count * START_MENU_ITEM_H + START_MENU_PADDING * 2;
    int menu_x = 0;
    int menu_y = framebuffer.fb_height - TASKBAR_HEIGHT - menu_h;

    for (int x = menu_x; x < menu_x + START_MENU_WIDTH; x++) {
        for (int y = menu_y; y < menu_y + menu_h; y++) {
            desktop_set_pixel(x, y, 0x2a2a2a);
        }
    }

    desktop_draw_line(menu_x, menu_y, menu_x + START_MENU_WIDTH, menu_y, 0x555555);
    desktop_draw_line(menu_x + START_MENU_WIDTH - 1, menu_y, menu_x + START_MENU_WIDTH - 1, menu_y + menu_h, 0x555555);
    desktop_draw_line(menu_x, menu_y, menu_x, menu_y + menu_h, 0x555555);

    for (int i = 0; i < count; i++) {
        int ix = menu_x + START_MENU_PADDING;
        int iy = menu_y + START_MENU_PADDING + i * START_MENU_ITEM_H;
        int iw = START_MENU_WIDTH - START_MENU_PADDING * 2;
        int ih = START_MENU_ITEM_H;

        s_menu_items[i].x = ix;
        s_menu_items[i].y = iy;
        s_menu_items[i].width = iw;
        s_menu_items[i].height = ih;

        for (int x = ix; x < ix + iw; x++) {
            for (int y = iy; y < iy + ih; y++) {
                desktop_set_pixel(x, y, 0x3a3a4e);
            }
        }

        desktop_draw_line(ix, iy, ix + iw, iy, 0x555577);
        desktop_draw_line(ix, iy + ih - 1, ix + iw, iy + ih - 1, 0x333344);
        desktop_draw_line(ix, iy, ix, iy + ih, 0x555577);
        desktop_draw_line(ix + iw - 1, iy, ix + iw - 1, iy + ih, 0x333344);

        desktop_draw_string(&font, ix + 8, iy + 4, window_definitions[i]->name, 0xaaffaa, 0x3a3a4e);
    }
}

void desktop_draw_taskbar(void) {
    int window_count = window_get_count();
    int focused_window = window_get_focused();
    
    int taskbar_y = framebuffer.fb_height - TASKBAR_HEIGHT;
    for (int x = 0; x < framebuffer.fb_width; x++) {
        for (int y = taskbar_y; y < framebuffer.fb_height; y++) {
            desktop_set_pixel(x, y, 0x2a2a2a);
        }
    }
    
    desktop_draw_line(0, taskbar_y, framebuffer.fb_width, taskbar_y, 0x555555);

    s_start_button.x = TASKBAR_PADDING;
    s_start_button.y = taskbar_y + TASKBAR_PADDING;
    s_start_button.width = START_BUTTON_WIDTH;
    s_start_button.height = TASKBAR_HEIGHT - TASKBAR_PADDING * 2;

    uint32_t start_bg = s_start_menu_open ? 0x4488ff : 0x445566;
    for (int x = s_start_button.x; x < s_start_button.x + s_start_button.width; x++) {
        for (int y = s_start_button.y; y < s_start_button.y + s_start_button.height; y++) {
            desktop_set_pixel(x, y, start_bg);
        }
    }
    desktop_draw_line(s_start_button.x, s_start_button.y, s_start_button.x + s_start_button.width, s_start_button.y, 0x666688);
    desktop_draw_line(s_start_button.x, s_start_button.y + s_start_button.height - 1, s_start_button.x + s_start_button.width, s_start_button.y + s_start_button.height - 1, 0x333344);
    desktop_draw_line(s_start_button.x, s_start_button.y, s_start_button.x, s_start_button.y + s_start_button.height, 0x666688);
    desktop_draw_line(s_start_button.x + s_start_button.width - 1, s_start_button.y, s_start_button.x + s_start_button.width - 1, s_start_button.y + s_start_button.height, 0x333344);
    desktop_draw_string(&font, s_start_button.x + 4, taskbar_y + 4, "Start", 0xffffff, start_bg);

    int items_start_x = TASKBAR_PADDING + START_BUTTON_WIDTH + TASKBAR_PADDING;
    
    int year, month, day, hour, minute, second;
    from_unix_time((unsigned long int)time(NULL), &year, &month, &day, &hour, &minute, &second);
    char clock_buf[16] = { 0 };
    sprintf(clock_buf, "%d:%d:%d", hour, minute, second);
    int clock_char_w = 8 * 8;
    int clock_x = framebuffer.fb_width - clock_char_w - TASKBAR_PADDING;
    desktop_draw_string(&font, clock_x, taskbar_y + 4, clock_buf, 0xffffff, 0x2a2a2a);

    int available_width = clock_x - items_start_x - TASKBAR_PADDING;
    int item_width = available_width / (window_count > 0 ? window_count : 1);
    if (item_width > TASKBAR_ITEM_MIN_WIDTH) {
        item_width = TASKBAR_ITEM_MIN_WIDTH;
    }
    
    int x_pos = items_start_x;
    s_taskbar_item_count = 0;
    for (int i = 0; i < window_count; i++) {
        window_instance_t* w = window_get(i);
        if (!w) {
            continue;
        }
        
        uint32_t item_bg;
        uint32_t item_text;
        if (w->is_minimized) {
            item_bg = 0x333333;
            item_text = 0x888888;
        } else if (i == focused_window) {
            item_bg = 0x4488ff;
            item_text = 0xffffff;
        } else {
            item_bg = 0x444444;
            item_text = 0xaaaaaa;
        }
        
        for (int x = x_pos; x < x_pos + item_width - 2; x++) {
            for (int y = taskbar_y + TASKBAR_PADDING; y < framebuffer.fb_height - TASKBAR_PADDING; y++) {
                desktop_set_pixel(x, y, item_bg);
            }
        }
        
        desktop_draw_line(x_pos, taskbar_y + TASKBAR_PADDING, x_pos + item_width - 2, taskbar_y + TASKBAR_PADDING, 0x666666);
        desktop_draw_line(x_pos, framebuffer.fb_height - TASKBAR_PADDING - 1, x_pos + item_width - 2, framebuffer.fb_height - TASKBAR_PADDING - 1, 0x333333);
        desktop_draw_line(x_pos, taskbar_y + TASKBAR_PADDING, x_pos, framebuffer.fb_height - TASKBAR_PADDING, 0x666666);
        desktop_draw_line(x_pos + item_width - 2, taskbar_y + TASKBAR_PADDING, x_pos + item_width - 2, framebuffer.fb_height - TASKBAR_PADDING, 0x333333);
        
        desktop_draw_string(&font, x_pos + 4, taskbar_y + 4, w->title, item_text, item_bg);
        
        s_taskbar_items[s_taskbar_item_count].x = x_pos;
        s_taskbar_items[s_taskbar_item_count].y = taskbar_y + TASKBAR_PADDING;
        s_taskbar_items[s_taskbar_item_count].width = item_width - 2;
        s_taskbar_items[s_taskbar_item_count].height = TASKBAR_HEIGHT - TASKBAR_PADDING * 2;
        s_taskbar_item_window_idx[s_taskbar_item_count] = i;
        s_taskbar_item_count++;
        
        x_pos += item_width;
    }
}

bool desktop_taskbar_handle_click(int x, int y) {
    for (int i = 0; i < s_taskbar_item_count; i++) {
        click_area_t* area = &s_taskbar_items[i];
        if (x >= area->x && x < area->x + area->width &&
            y >= area->y && y < area->y + area->height) {
            int win_idx = s_taskbar_item_window_idx[i];
            window_instance_t* w = window_get(win_idx);
            if (w) {
                if (w->is_minimized) {
                    w->is_minimized = false;
                    w->is_dirty = true;
                    window_set_focused(win_idx);
                } else if (win_idx == window_get_focused()) {
                    w->is_minimized = true;
                    w->is_dirty = true;
                    window_set_focused(-1);
                } else {
                    window_set_focused(win_idx);
                }
            }
            return true;
        }
    }
    return false;
}

void desktop_draw_all(void) {
    desktop_draw_background();
    
    int window_count = window_get_count();
    int focused_window = window_get_focused();
    
    for (int i = 0; i < window_count; i++) {
        if (i != focused_window) {
            window_instance_t* w = window_get(i);
            if (w && !w->is_minimized) {
                desktop_draw_window_chrome(w);
                if (w->draw) {
                    w->draw(w);
                }
                w->is_dirty = false;
            }
        }
    }
    
    if (focused_window >= 0 && focused_window < window_count) {
        window_instance_t* w = window_get(focused_window);
        if (w && !w->is_minimized) {
            desktop_draw_window_chrome(w);
            if (w->draw) {
                w->draw(w);
            }
            w->is_dirty = false;
        }
    }
    
    desktop_draw_taskbar();

    if (s_start_menu_open) {
        desktop_draw_start_menu();
    }
}

void desktop_poll_messages(void) {
    char path[256] = { 0 };
    if (message_recv(TOPIC_DESKTOP_OPEN_FILE, path, sizeof(path)) > 0) {
        desktop_open_file(path);
    }
}
