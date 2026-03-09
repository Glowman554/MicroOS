#pragma once

#include <types.h>

typedef struct {
    click_area_t* files;
    int offset;
    int max_rows;
    bool fs_mode;
    char cwd[64];
} explorer_state_t;

void explorer_init(window_instance_t* w);
void explorer_update(window_instance_t* w, event_t* event);
void explorer_draw(window_instance_t* w);
void explorer_cleanup(window_instance_t* w);
void register_explorer_window(void);

extern window_definition_t explorer_definition;

