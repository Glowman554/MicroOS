#include <test.h>
#include <window_helpers.h>
#include <window.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

void test_init(window_instance_t* w) {
    test_state_t* state = malloc(sizeof(test_state_t));
    strcpy(state->text, "Click to change!");
    w->state = state;
    w->title_bar_color = 0x44ff44;
}

void test_update(window_instance_t* w, event_t* event) {
    test_state_t* state = (test_state_t*)w->state;
    
    if (event->type == EVENT_MOUSE_CLICK) {
        if (event->button == MOUSE_BUTTON_LEFT) {
            if (strcmp(state->text, "Click to change!") == 0) {
                memset(state->text, 0, sizeof(state->text));
                strcpy(state->text, "You clicked!");
            } else {
                memset(state->text, 0, sizeof(state->text));
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

void test_draw(window_instance_t* w) {

    test_state_t* state = (test_state_t*)w->state;
    window_draw_string(w, 10, 30, state->text, 0xffffff);
}

void test_cleanup(window_instance_t* w) {
    if (w->state) {
        free(w->state);
        w->state = NULL;
    }
}

window_definition_t test_definition = {
    .name = "Test Window",
    .register_window = register_test_window,
};

void register_test_window(void) {
    window_add(200, 150, 280, 220, "Test Window", 0x445533, test_init, test_update, test_draw, test_cleanup);
}

void init() {
    register_window(&test_definition);
}
