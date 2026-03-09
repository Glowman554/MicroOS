#include "sysctl.h"
#include <window.h>
#include <window_helpers.h>
#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/raminfo.h>
#include <sys/env.h>
#include <buildin/unix_time.h>

extern psf1_font_t font;
extern fpic_image_t reboot_button;
extern fpic_image_t shutdown_button;

#define is_kb(x) ((x) >= 1024)
#define is_mb(x) ((x) >= 1024 * 1024)
#define is_gb(x) ((x) >= 1024 * 1024 * 1024)
#define to_kb(x) ((x) / 1024)
#define to_mb(x) ((x) / 1024 / 1024)
#define to_gb(x) ((x) / 1024 / 1024 / 1024)

static void format_memory_usage(char* out_buf, uint32_t usage) {
    if (is_gb(usage)) {
        sprintf(out_buf, "%d,%d GB", to_gb(usage), to_mb(usage) % 1024);
    } else if (is_mb(usage)) {
        sprintf(out_buf, "%d,%d MB", to_mb(usage), to_kb(usage) % 1024);
    } else if (is_kb(usage)) {
        sprintf(out_buf, "%d,%d KB", to_kb(usage), usage % 1024);
    } else {
        sprintf(out_buf, "%d B", usage);
    }
}

void sysctl_init(window_instance_t* w) {
    sysctl_state_t* state = malloc(sizeof(sysctl_state_t));

    int btn_w = (int)reboot_button.width;
    int btn_h = (int)reboot_button.height;

    state->reboot_area.x   = w->width - btn_w - 4;
    state->reboot_area.y   = TITLE_BAR_HEIGHT;
    state->reboot_area.width  = btn_w;
    state->reboot_area.height = btn_h;

    state->shutdown_area.x  = w->width - btn_w - 4;
    state->shutdown_area.y  = TITLE_BAR_HEIGHT + btn_h + 4;
    state->shutdown_area.width  = btn_w;
    state->shutdown_area.height = btn_h;

    w->state = state;
    w->title_bar_color = 0x553366;
    w->is_dirty = true;
}

void sysctl_update(window_instance_t* w, event_t* event) {
    sysctl_state_t* state = (sysctl_state_t*)w->state;

    w->is_dirty = true;

    if (event->type == EVENT_MOUSE_CLICK && event->button == MOUSE_BUTTON_LEFT) {
        click_area_t* rb = &state->reboot_area;
        click_area_t* sd = &state->shutdown_area;

        if (event->x >= rb->x && event->x < rb->x + rb->width &&
            event->y >= rb->y && event->y < rb->y + rb->height) {
            env(SYS_PWR_RESET_ID);
        }
        if (event->x >= sd->x && event->x < sd->x + sd->width &&
            event->y >= sd->y && event->y < sd->y + sd->height) {
            env(SYS_PWR_SHUTDOWN_ID);
        }
    }
}

void sysctl_draw(window_instance_t* w) {
    sysctl_state_t* state = (sysctl_state_t*)w->state;

    for (int x = 0; x < w->width; x++) {
        for (int y = TITLE_BAR_HEIGHT; y < w->height; y++) {
            desktop_set_pixel(w->x + x, w->y + y, 0x1a0a2e);
        }
    }

    click_area_t* rb = &state->reboot_area;
    desktop_draw_fpic(&reboot_button, w->x + rb->x, w->y + rb->y);

    click_area_t* sd = &state->shutdown_area;
    desktop_draw_fpic(&shutdown_button, w->x + sd->x, w->y + sd->y);

    char timebuf[128] = { 0 };
    unix_time_to_string(time(NULL), timebuf);

    char timemsbuf[32] = { 0 };
    sprintf(timemsbuf, "%d ms", time_ms());

    uint32_t mem_free, mem_used;
    raminfo(&mem_free, &mem_used);

    char total_str[32] = { 0 };
    char free_str[32]  = { 0 };
    char used_str[32]  = { 0 };
    format_memory_usage(total_str, mem_free + mem_used);
    format_memory_usage(free_str,  mem_free);
    format_memory_usage(used_str,  mem_used);

    int line = 0;
    window_draw_string(w, 4, line * 16 + 4, "TIME:",   0xaaaaff);
    line++;
    window_draw_string(w, 4, line * 16 + 4, timebuf,   0xffffff);
    line++;
    line++;
    window_draw_string(w, 4, line * 16 + 4, "TIMEMS:", 0xaaaaff);
    line++;
    window_draw_string(w, 4, line * 16 + 4, timemsbuf, 0xffffff);
    line++;
    line++;
    window_draw_string(w, 4, line * 16 + 4, "TOTAL:",  0xaaaaff);
    line++;
    window_draw_string(w, 4, line * 16 + 4, total_str, 0xffffff);
    line++;
    window_draw_string(w, 4, line * 16 + 4, "FREE:",   0xaaaaff);
    line++;
    window_draw_string(w, 4, line * 16 + 4, free_str,  0xffffff);
    line++;
    window_draw_string(w, 4, line * 16 + 4, "USED:",   0xaaaaff);
    line++;
    window_draw_string(w, 4, line * 16 + 4, used_str,  0xffffff);
}

void sysctl_cleanup(window_instance_t* w) {
    if (w->state) {
        free(w->state);
        w->state = NULL;
    }
}

window_definition_t sysctl_definition = {
    .name = "System Control",
    .register_window = register_sysctl_window,
};

void register_sysctl_window(void) {
    window_add(60, 60, 280, 260, "System Control", 0x1a0a2e, sysctl_init, sysctl_update, sysctl_draw, sysctl_cleanup);
}
