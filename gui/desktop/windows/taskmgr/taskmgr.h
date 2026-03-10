#pragma once

#include <types.h>
#include <button.h>
#include <sys/spawn.h>

#define TASKMGR_MAX_TASKS 32

typedef struct {
    task_list_t tasks[TASKMGR_MAX_TASKS];
    button_t kill_buttons[TASKMGR_MAX_TASKS];
    int task_count;
} taskmgr_state_t;

void taskmgr_init(window_instance_t* w);
void taskmgr_update(window_instance_t* w, event_t* event);
void taskmgr_draw(window_instance_t* w);
void taskmgr_cleanup(window_instance_t* w);
void register_taskmgr_window(void);

extern window_definition_t taskmgr_definition;
