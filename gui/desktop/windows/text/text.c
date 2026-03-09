#include "text.h"
#include <window_helpers.h>
#include <window.h>
#include <stdlib.h>
#include <string.h>

void text_init(window_instance_t* w) {
    text_state_t* state = malloc(sizeof(text_state_t));
    strcpy(state->text, "Click to change!");
    w->state = state;
    w->title_bar_color = 0x44ff44;
}

void text_update(window_instance_t* w, event_t* event) {
    text_state_t* state = (text_state_t*)w->state;
    
    if (event->type == EVENT_MOUSE_CLICK) {
        if (event->button == MOUSE_BUTTON_LEFT) {
            if (strcmp(state->text, "Click to change!") == 0) {
                strcpy(state->text, "You clicked!");
            } else {
                strcpy(state->text, "Click to change!");
            }
            w->is_dirty = true;
        } else if (event->button == MOUSE_BUTTON_RIGHT) {
            strcpy(state->text, "Right clicked!");
            w->is_dirty = true;
        } else if (event->button == MOUSE_BUTTON_MIDDLE) {
            strcpy(state->text, "Middle clicked!");
            w->is_dirty = true;
        }
    }
}

void text_draw(window_instance_t* w) {
    text_state_t* state = (text_state_t*)w->state;
    window_draw_string(w, 10, 30, state->text, 0xffffff);
}

void text_cleanup(window_instance_t* w) {
    if (w->state) {
        free(w->state);
        w->state = NULL;
    }
}

// Text window definition
window_definition_t text_definition = {
    .name = "Text Window",
    .register_window = register_text_window,
};

void register_text_window(void) {
    window_add(200, 150, 280, 220, "Text Window", 0x445533,
               text_init, text_update, text_draw, text_cleanup);
}
