#include "taskmgr.h"
#include <window.h>
#include <window_helpers.h>
#include <graphics.h>
#include <button.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/raminfo.h>

extern psf1_font_t font;

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

static void on_kill_task(window_instance_t* w, void* userdata) {
    int pid = (int)(long)userdata;
    kill(pid);
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

    for (int i = 0; i < state->task_count; i++) {
        int row_y = 20 + i * 16;
        int btn_x = w->width - 7 * 8;
        button_init(&state->kill_buttons[i], btn_x, row_y, 6 * 8, 16, "Kill", on_kill_task, (void*)(long)state->tasks[i].pid);
        state->kill_buttons[i].bg_color = 0x662222;
        state->kill_buttons[i].hover_color = 0xaa3333;
        state->kill_buttons[i].text_color = 0xff8888;
        button_handle_event(&state->kill_buttons[i], w, event);
    }
}

void taskmgr_draw(window_instance_t* w) {
    taskmgr_state_t* state = (taskmgr_state_t*)w->state;

    window_draw_string(w, 4, 4, "PID  Term Name", 0x88ffaa);
    window_draw_string(w, w->width - 6 * 8, 4, "Kill", 0x88ffaa);

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

        button_draw(&state->kill_buttons[i], w);
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
