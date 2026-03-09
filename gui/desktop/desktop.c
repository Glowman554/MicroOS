#include <desktop.h>
#include <graphics.h>
#include <window.h>
#include <types.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <buildin/unix_time.h>

#define MAX_ASSOCS 16

typedef struct {
    char ext[16];
    void (*open_fn)(const char* path);
} file_assoc_t;

static file_assoc_t s_assocs[MAX_ASSOCS];
static int s_assoc_count = 0;

void desktop_register_file_assoc(const char* ext, void (*open_fn)(const char* path)) {
    if (s_assoc_count >= MAX_ASSOCS) {
        return;
    }
    int ext_len = strnlen(ext, sizeof(s_assocs[0].ext) - 1);
    memcpy(s_assocs[s_assoc_count].ext, ext, ext_len);
    s_assocs[s_assoc_count].ext[ext_len] = '\0';
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
            s_assocs[i].open_fn(path);
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
    
    desktop_draw_line(w->x, w->y, w->x + w->width, w->y, 0x666666);
    desktop_draw_line(w->x, w->y + TITLE_BAR_HEIGHT - 1, w->x + w->width, w->y + TITLE_BAR_HEIGHT - 1, 0x666666);
    desktop_draw_line(w->x, w->y, w->x, w->y + TITLE_BAR_HEIGHT - 1, 0x666666);
    desktop_draw_line(w->x + w->width - 1, w->y, w->x + w->width - 1, w->y + TITLE_BAR_HEIGHT - 1, 0x666666);
    
    desktop_draw_string(&font, w->x + 4, w->y + 2, w->title, 0xffffff, w->title_bar_color);
    
    w->close_button.x = w->x + w->width - BUTTON_SIZE - 2;
    w->close_button.y = w->y + 2;
    w->close_button.width = BUTTON_SIZE;
    w->close_button.height = BUTTON_SIZE;
    
    desktop_draw_line(w->close_button.x, w->close_button.y, w->close_button.x + BUTTON_SIZE, w->close_button.y, 0xff0000);
    desktop_draw_line(w->close_button.x, w->close_button.y, w->close_button.x, w->close_button.y + BUTTON_SIZE, 0xff0000);
    desktop_draw_line(w->close_button.x + BUTTON_SIZE, w->close_button.y, w->close_button.x + BUTTON_SIZE, w->close_button.y + BUTTON_SIZE, 0xff0000);
    desktop_draw_line(w->close_button.x, w->close_button.y + BUTTON_SIZE, w->close_button.x + BUTTON_SIZE, w->close_button.y + BUTTON_SIZE, 0xff0000);
    
    for (int i = w->x + BORDER_WIDTH; i < w->x + w->width - BORDER_WIDTH; i++) {
        for (int j = w->y + TITLE_BAR_HEIGHT; j < w->y + w->height - BORDER_WIDTH; j++) {
            desktop_set_pixel(i, j, w->bg_color);
        }
    }
    
    desktop_draw_line(w->x, w->y + TITLE_BAR_HEIGHT, w->x + w->width, w->y + TITLE_BAR_HEIGHT, 0x666666);
    desktop_draw_line(w->x, w->y + w->height - BORDER_WIDTH, w->x + w->width, w->y + w->height - BORDER_WIDTH, 0x666666);
    desktop_draw_line(w->x, w->y + TITLE_BAR_HEIGHT, w->x, w->y + w->height - BORDER_WIDTH, 0x666666);
    desktop_draw_line(w->x + w->width - BORDER_WIDTH, w->y + TITLE_BAR_HEIGHT, w->x + w->width - BORDER_WIDTH, w->y + w->height - BORDER_WIDTH, 0x666666);
    
    for (int i = 0; i < RESIZE_BORDER; i++) {
        desktop_draw_line(w->x + i, w->y + TITLE_BAR_HEIGHT + i, w->x + w->width - i, w->y + TITLE_BAR_HEIGHT + i, 0x888888);
        desktop_draw_line(w->x + i, w->y + w->height - 1 - i, w->x + w->width - i, w->y + w->height - 1 - i, 0x888888);
        desktop_draw_line(w->x + i, w->y + TITLE_BAR_HEIGHT, w->x + i, w->y + w->height, 0x888888);
        desktop_draw_line(w->x + w->width - 1 - i, w->y + TITLE_BAR_HEIGHT, w->x + w->width - 1 - i, w->y + w->height, 0x888888);
    }
}

#define TASKBAR_HEIGHT 24
#define TASKBAR_PADDING 4
#define TASKBAR_ITEM_MIN_WIDTH 80

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
    
    int year, month, day, hour, minute, second;
    from_unix_time((unsigned long int)time(NULL), &year, &month, &day, &hour, &minute, &second);
    char clock_buf[16] = { 0 };
    sprintf(clock_buf, "%d:%d:%d", hour, minute, second);
    int clock_char_w = 8 * 8;
    int clock_x = framebuffer.fb_width - clock_char_w - TASKBAR_PADDING;
    desktop_draw_string(&font, clock_x, taskbar_y + 4, clock_buf, 0xffffff, 0x2a2a2a);

    int available_width = framebuffer.fb_width - TASKBAR_PADDING * 2 - clock_char_w - TASKBAR_PADDING * 2;
    int item_width = available_width / (window_count > 0 ? window_count : 1);
    if (item_width > TASKBAR_ITEM_MIN_WIDTH) {
        item_width = TASKBAR_ITEM_MIN_WIDTH;
    }
    
    int x_pos = TASKBAR_PADDING;
    for (int i = 0; i < window_count; i++) {
        window_instance_t* w = window_get(i);
        if (!w) {
            continue;
        }
        
        uint32_t item_bg = (i == focused_window) ? 0x4488ff : 0x444444;
        uint32_t item_text = (i == focused_window) ? 0xffffff : 0xaaaaaa;
        
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
        
        x_pos += item_width;
    }
}

void desktop_draw_all(void) {
    desktop_draw_background();
    
    int window_count = window_get_count();
    int focused_window = window_get_focused();
    
    for (int i = 0; i < window_count; i++) {
        if (i != focused_window) {
            window_instance_t* w = window_get(i);
            if (w) {
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
        if (w) {
            desktop_draw_window_chrome(w);
            if (w->draw) {
                w->draw(w);
            }
            w->is_dirty = false;
        }
    }
    
    desktop_draw_taskbar();
}
