#include "launcher.h"
#include <window.h>
#include <window_helpers.h>
#include <windows.h>
#include <button.h>
#include <non-standard/buildin/data/array.h>
#include <stdlib.h>

#define LAUNCHER_BUTTON_HEIGHT 24
#define LAUNCHER_BUTTON_SPACING 6
#define LAUNCHER_PADDING 10

void on_launcher_button(window_instance_t* w, void* userdata) {
    window_definition_t* def = (window_definition_t*)userdata;
    def->register_window();
}

void launcher_init(window_instance_t* w) {
    launcher_state_t* state = malloc(sizeof(launcher_state_t));
    state->button_count = (int)array_length(window_definitions);
    state->buttons = malloc(sizeof(button_t) * state->button_count);
    
    for (int i = 0; i < state->button_count; i++) {
        button_init(&state->buttons[i], LAUNCHER_PADDING, 30 + i * (LAUNCHER_BUTTON_HEIGHT + LAUNCHER_BUTTON_SPACING), w->width - (LAUNCHER_PADDING * 2), LAUNCHER_BUTTON_HEIGHT, window_definitions[i]->name, on_launcher_button, window_definitions[i]);
        state->buttons[i].text_color = 0xaaffaa;
    }
    
    w->state = state;
    w->title_bar_color = 0xff6600;
}

void launcher_update(window_instance_t* w, event_t* event) {
    launcher_state_t* state = (launcher_state_t*)w->state;
    
    for (int i = 0; i < state->button_count; i++) {
        button_handle_event(&state->buttons[i], w, event);
    }
}

void launcher_draw(window_instance_t* w) {
    launcher_state_t* state = (launcher_state_t*)w->state;
    
    window_draw_string(w, LAUNCHER_PADDING, 5, "Available Windows:", 0xffffff);
    
    for (int i = 0; i < state->button_count; i++) {
        button_draw(&state->buttons[i], w);
    }
}

void launcher_cleanup(window_instance_t* w) {
    launcher_state_t* state = (launcher_state_t*)w->state;
    if (state) {
        if (state->buttons) {
            free(state->buttons);
        }
        free(state);
        w->state = NULL;
    }
}

void register_launcher_window(void) {
    window_add(10, 10, 300, 400, "Launcher", 0x333333, launcher_init, launcher_update, launcher_draw, launcher_cleanup);
}

window_definition_t launcher_definition = {
    .name = "Launcher",
    .register_window = register_launcher_window,
};