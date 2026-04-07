#include "counter.h"
#include <window_helpers.h>
#include <window.h>
#include <button.h>
#include <stdlib.h>
#include <stdio.h>

static void on_decrement(window_instance_t* w, void* userdata) {
    counter_state_t* state = (counter_state_t*)w->state;
    state->count--;
}

static void on_increment(window_instance_t* w, void* userdata) {
    counter_state_t* state = (counter_state_t*)w->state;
    state->count++;
}

static void on_reset(window_instance_t* w, void* userdata) {
    counter_state_t* state = (counter_state_t*)w->state;
    (void)userdata;
    state->count = 0;
}

void counter_init(window_instance_t* w) {
    counter_state_t* state = malloc(sizeof(counter_state_t));
    state->count = 0;

    button_init(&state->decrement_btn, 10, 40, 100, 28, "- Minus", on_decrement, NULL);
    button_init(&state->increment_btn, 120, 40, 100, 28, "+ Plus", on_increment, NULL);
    button_init(&state->reset_btn, 10, 76, 210, 28, "Reset", on_reset, NULL);

    w->state = state;
    w->title_bar_color = 0x4444ff;
}

void counter_update(window_instance_t* w, event_t* event) {
    counter_state_t* state = (counter_state_t*)w->state;

    button_handle_event(&state->decrement_btn, w, event);
    button_handle_event(&state->increment_btn, w, event);
    button_handle_event(&state->reset_btn, w, event);
}

void counter_draw(window_instance_t* w) {
    counter_state_t* state = (counter_state_t*)w->state;

    char buf[32];
    sprintf(buf, "Count: %d", state->count);
    window_draw_string(w, 10, 10, buf, 0xffffff);

    button_draw(&state->decrement_btn, w);
    button_draw(&state->increment_btn, w);
    button_draw(&state->reset_btn, w);
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
