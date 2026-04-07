#pragma once

#include <types.h>
#include <button.h>
#include <non-standard/net/ipv4.h>

typedef struct {
    int interface;
    int nic_valid;
    nic_content_t nic_data;
    button_t up_btn;
    button_t down_btn;
    button_t dhcp_btn;
} netinfo_state_t;

void netinfo_init(window_instance_t* w);
void netinfo_update(window_instance_t* w, event_t* event);
void netinfo_draw(window_instance_t* w);
void netinfo_cleanup(window_instance_t* w);
void register_netinfo_window(void);

extern window_definition_t netinfo_definition;
