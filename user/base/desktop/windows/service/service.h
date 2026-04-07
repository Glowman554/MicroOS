#pragma once

#include <types.h>
#include <button.h>
#include <non-standard/sys/message.h>

typedef struct {
    service_list_reply_t list;
    button_t refresh_btn;
    button_t start_buttons[MAX_SERVICES];
    button_t stop_buttons[MAX_SERVICES];
    button_t restart_buttons[MAX_SERVICES];
    int has_list;
    int waiting_for_list;
    int waiting_for_op;
    long next_poll_ms;
    char status_text[96];
} service_window_state_t;

void service_window_init(window_instance_t* w);
void service_window_update(window_instance_t* w, event_t* event);
void service_window_draw(window_instance_t* w);
void service_window_cleanup(window_instance_t* w);
void register_service_window(void);

extern window_definition_t service_window_definition;