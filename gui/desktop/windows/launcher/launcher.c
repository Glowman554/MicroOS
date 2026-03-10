#include "launcher.h"
#include <window.h>
#include <window_helpers.h>
#include <windows.h>
#include <buildin/array.h>
#include <stdlib.h>

#define LAUNCHER_BUTTON_HEIGHT 24
#define LAUNCHER_BUTTON_SPACING 6
#define LAUNCHER_PADDING 10

void launcher_init(window_instance_t* w) {
    launcher_state_t* state = malloc(sizeof(launcher_state_t));
    state->button_count = (int)array_length(window_definitions);
    state->buttons = malloc(sizeof(click_area_t) * state->button_count);
    
    for (int i = 0; i < state->button_count; i++) {
        state->buttons[i].x = LAUNCHER_PADDING;
        state->buttons[i].y = 30 + i * (LAUNCHER_BUTTON_HEIGHT + LAUNCHER_BUTTON_SPACING);
        state->buttons[i].width = w->width - (LAUNCHER_PADDING * 2);
        state->buttons[i].height = LAUNCHER_BUTTON_HEIGHT;
    }
    
    w->state = state;
    w->title_bar_color = 0xff6600;
}

void launcher_update(window_instance_t* w, event_t* event) {
    launcher_state_t* state = (launcher_state_t*)w->state;
    
    if (event->type == EVENT_MOUSE_CLICK && event->button == MOUSE_BUTTON_LEFT) {
        for (int i = 0; i < state->button_count; i++) {
            click_area_t* btn = &state->buttons[i];
            if (event->x >= btn->x && event->x < btn->x + btn->width &&
                event->y >= TITLE_BAR_HEIGHT + btn->y && event->y < TITLE_BAR_HEIGHT + btn->y + btn->height) {
                
                window_definition_t* def = window_definitions[i];
                def->register_window();
                break;
            }
        }
    }
}

void launcher_draw(window_instance_t* w) {
    launcher_state_t* state = (launcher_state_t*)w->state;
    
    window_draw_string(w, LAUNCHER_PADDING, 5, "Available Windows:", 0xffffff);
    
    for (int i = 0; i < state->button_count; i++) {
        click_area_t* btn = &state->buttons[i];
        
        for (int x = btn->x; x < btn->x + btn->width; x++) {
            for (int y = btn->y; y < btn->y + btn->height; y++) {
                window_set_pixel(w, x, TITLE_BAR_HEIGHT + y, 0x445566);
            }
        }
        
        window_draw_line(w, btn->x, btn->y, btn->x + btn->width, btn->y, 0x666688);
        window_draw_line(w, btn->x, btn->y + btn->height - 1, btn->x + btn->width, btn->y + btn->height - 1, 0x666688);
        window_draw_line(w, btn->x, btn->y, btn->x, btn->y + btn->height, 0x666688);
        window_draw_line(w, btn->x + btn->width - 1, btn->y, btn->x + btn->width - 1, btn->y + btn->height, 0x666688);
        
        window_draw_string(w, btn->x + 8, btn->y + 4, window_definitions[i]->name, 0xaaffaa);
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