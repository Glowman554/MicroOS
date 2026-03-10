#pragma once

#include <types.h>
#include <button.h>

typedef struct {
    button_t* buttons;
    int button_count;
} launcher_state_t;

void launcher_init(window_instance_t* w);
void launcher_update(window_instance_t* w, event_t* event);
void launcher_draw(window_instance_t* w);
void launcher_cleanup(window_instance_t* w);

void register_launcher_window();

extern window_definition_t launcher_definition;

