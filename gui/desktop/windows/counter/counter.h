#pragma once

#include <types.h>
#include <button.h>

typedef struct {
    int count;
    button_t increment_btn;
    button_t decrement_btn;
    button_t reset_btn;
} counter_state_t;

void counter_init(window_instance_t* w);
void counter_update(window_instance_t* w, event_t* event);
void counter_draw(window_instance_t* w);
void counter_cleanup(window_instance_t* w);
void register_counter_window(void);

extern window_definition_t counter_definition;

