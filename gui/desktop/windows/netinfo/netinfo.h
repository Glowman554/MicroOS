#pragma once

#include <types.h>
#include <net/ipv4.h>

typedef struct {
    int interface;
    int nic_valid;
    nic_content_t nic_data;
    click_area_t up_area;
    click_area_t down_area;
    click_area_t dhcp_area;
} netinfo_state_t;

void netinfo_init(window_instance_t* w);
void netinfo_update(window_instance_t* w, event_t* event);
void netinfo_draw(window_instance_t* w);
void netinfo_cleanup(window_instance_t* w);
void register_netinfo_window(void);

extern window_definition_t netinfo_definition;
