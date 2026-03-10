#pragma once

#include <types.h>
#include <button.h>

typedef struct {
    button_t reboot_btn;
    button_t shutdown_btn;
} sysctl_state_t;

void sysctl_init(window_instance_t* w);
void sysctl_update(window_instance_t* w, event_t* event);
void sysctl_draw(window_instance_t* w);
void sysctl_cleanup(window_instance_t* w);
void register_sysctl_window(void);

extern window_definition_t sysctl_definition;
