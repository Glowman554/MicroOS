#pragma once

#include <types.h>

typedef struct {
    click_area_t reboot_area;
    click_area_t shutdown_area;
} sysctl_state_t;

void sysctl_init(window_instance_t* w);
void sysctl_update(window_instance_t* w, event_t* event);
void sysctl_draw(window_instance_t* w);
void sysctl_cleanup(window_instance_t* w);
void register_sysctl_window(void);

extern window_definition_t sysctl_definition;
