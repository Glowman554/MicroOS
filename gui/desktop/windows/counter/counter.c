#include "counter.h"
#include <window_helpers.h>
#include <window.h>
#include <stdlib.h>
#include <stdio.h>

void counter_init(window_instance_t* w) {
    counter_state_t* state = malloc(sizeof(counter_state_t));
    state->count = 0;
    w->state = state;
    w->title_bar_color = 0x4444ff;
}

void counter_update(window_instance_t* w, event_t* event) {
    counter_state_t* state = (counter_state_t*)w->state;
    
    if (event->type == EVENT_MOUSE_CLICK) {
        if (event->button == MOUSE_BUTTON_LEFT) {
            if (event->x < w->width / 2) {
                state->count--;
            } else {
                state->count++;
            }
            w->is_dirty = true;
        } else if (event->button == MOUSE_BUTTON_RIGHT) {
            state->count = 0;
            w->is_dirty = true;
        } else if (event->button == MOUSE_BUTTON_MIDDLE) {
            state->count *= 2;
            w->is_dirty = true;
        }
    }
}

void counter_draw(window_instance_t* w) {
    counter_state_t* state = (counter_state_t*)w->state;
    
    char buf[32];
    sprintf(buf, "Count: %d", state->count);
    window_draw_string(w, 10, 20, buf, 0xffffff);
    
    window_draw_string(w, 10, 40, "Left: -/+", 0xcccccc);
    window_draw_string(w, 10, 56, "Right: reset", 0xcccccc);
    window_draw_string(w, 10, 72, "Middle: x2", 0xaaaaaa);
}

void counter_cleanup(window_instance_t* w) {
    if (w->state) {
        free(w->state);
        w->state = NULL;
    }
}

window_definition_t counter_definition = {
    .name = "Counter",
    .register_window = register_counter_window,
};

void register_counter_window(void) {
    window_add(50, 50, 300, 250, "Counter", 0x334455, counter_init, counter_update, counter_draw, counter_cleanup);
}
