#include "taskmgr.h"
#include <window.h>
#include <window_helpers.h>
#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/spawn.h>
#include <sys/raminfo.h>

extern psf1_font_t font;
extern fpic_image_t kill_button;

#define is_kb(x) ((x) >= 1024)
#define is_mb(x) ((x) >= 1024 * 1024)
#define is_gb(x) ((x) >= 1024 * 1024 * 1024)
#define to_kb(x) ((x) / 1024)
#define to_mb(x) ((x) / 1024 / 1024)
#define to_gb(x) ((x) / 1024 / 1024 / 1024)

static int format_memory_usage(char* out_buf, uint32_t usage) {
    if (is_gb(usage)) {
        return sprintf(out_buf, "%d,%d GB", to_gb(usage), to_mb(usage) % 1024);
    } else if (is_mb(usage)) {
        return sprintf(out_buf, "%d,%d MB", to_mb(usage), to_kb(usage) % 1024);
    } else if (is_kb(usage)) {
        return sprintf(out_buf, "%d,%d KB", to_kb(usage), usage % 1024);
    } else {
        return sprintf(out_buf, "%d B", usage);
    }
}

void taskmgr_init(window_instance_t* w) {
    taskmgr_state_t* state = malloc(sizeof(taskmgr_state_t));
    memset(state, 0, sizeof(taskmgr_state_t));
    state->task_count = 0;
    w->state = state;
    w->title_bar_color = 0x226644;
    w->is_dirty = true;
}

void taskmgr_update(window_instance_t* w, event_t* event) {
    taskmgr_state_t* state = (taskmgr_state_t*)w->state;

    state->task_count = get_task_list(state->tasks, TASKMGR_MAX_TASKS);
    w->is_dirty = true;

    if (event->type == EVENT_MOUSE_CLICK && event->button == MOUSE_BUTTON_LEFT) {
        for (int i = 0; i < state->task_count; i++) {
            click_area_t* btn = &state->kill_buttons[i];
            if (btn->width && btn->height &&
                event->x >= btn->x && event->x < btn->x + btn->width &&
                event->y >= btn->y && event->y < btn->y + btn->height) {
                kill(state->tasks[i].pid);
                w->is_dirty = true;
                break;
            }
        }
    }
}

void taskmgr_draw(window_instance_t* w) {
    taskmgr_state_t* state = (taskmgr_state_t*)w->state;

    for (int x = 0; x < w->width; x++) {
        for (int y = TITLE_BAR_HEIGHT; y < w->height; y++) {
            window_set_pixel(w, x, y, 0x111111);
        }
    }

    int kill_w = (int)kill_button.width;
    int kill_h = (int)kill_button.height;

    window_draw_string(w, 4, 4, "PID  Term Name", 0x88ffaa);
    window_draw_string(w, w->width - 4 * 8, 4, "Kill", 0x88ffaa);

    window_draw_line(w, 0, 18, w->width, 18, 0x336644);

    // Task rows
    for (int i = 0; i < state->task_count; i++) {
        int row_y = 20 + i * 16;

        char pid_buf[16] = { 0 };
        sprintf(pid_buf, "%d", state->tasks[i].pid);
        window_draw_string(w, 4, row_y + 1, pid_buf, 0xffffff);
        char term_buf[16] = { 0 };
        sprintf(term_buf, "%d", state->tasks[i].term);
        window_draw_string(w, 5 * 8, row_y + 1, term_buf, 0xffffff);

        window_draw_string(w, 10 * 8, row_y + 1, state->tasks[i].name, 0xffffff);


        int btn_x = w->width - kill_w - 4;
        int btn_y = row_y;

        state->kill_buttons[i].x = btn_x;
        state->kill_buttons[i].y = TITLE_BAR_HEIGHT + btn_y;
        state->kill_buttons[i].width = kill_w;
        state->kill_buttons[i].height = kill_h;

        window_draw_fpic(w, &kill_button, btn_x, TITLE_BAR_HEIGHT + btn_y);
    }

    uint32_t mem_free;
    uint32_t mem_used;
    raminfo(&mem_free, &mem_used);

    char mem_buf[80] = { 0 };
    char* ptr = mem_buf;
    ptr += sprintf(ptr, "Mem: ");
    ptr += format_memory_usage(ptr, mem_used);
    ptr += sprintf(ptr, " / ");
    ptr += format_memory_usage(ptr, mem_free + mem_used);

    int mem_y = w->height - TITLE_BAR_HEIGHT - 16;
    window_draw_line(w, 0, mem_y - 2, w->width, mem_y - 2, 0x336644);
    window_draw_string(w, 4, mem_y, mem_buf, 0x88ffaa);
}

void taskmgr_cleanup(window_instance_t* w) {
    if (w->state) {
        free(w->state);
        w->state = NULL;
    }
}

window_definition_t taskmgr_definition = {
    .name = "Task Manager",
    .register_window = register_taskmgr_window,
};

void register_taskmgr_window(void) {
    window_add(50, 50, 320, 300, "Task Manager", 0x111111, taskmgr_init, taskmgr_update, taskmgr_draw, taskmgr_cleanup);
}
