#include "netinfo.h"
#include <window.h>
#include <window_helpers.h>
#include <graphics.h>
#include <button.h>
#include <stdlib.h>
#include <stdio.h>
#include <non-standart/sys/file.h>
#include <non-standart/sys/spawn.h>
#include <non-standart/buildin/path.h>
#include <non-standart/net/ipv4.h>

extern psf1_font_t font;

static void netinfo_refresh(netinfo_state_t* state) {
    char buffer[16] = { 0 };
    sprintf(buffer, "dev:nic%d", state->interface);
    int fd = open(buffer, FILE_OPEN_MODE_READ);
    if (fd < 0) {
        state->nic_valid = 0;
        return;
    }
    close(fd);
    state->nic_data  = nic_read(state->interface);
    state->nic_valid = 1;
}

void on_netinfo_up(window_instance_t* w, void* userdata) {
    netinfo_state_t* state = (netinfo_state_t*)w->state;
    state->interface--;
    netinfo_refresh(state);
}

void on_netinfo_down(window_instance_t* w, void* userdata) {
    netinfo_state_t* state = (netinfo_state_t*)w->state;
    state->interface++;
    netinfo_refresh(state);
}

void on_netinfo_dhcp(window_instance_t* w, void* userdata) {
    netinfo_state_t* state = (netinfo_state_t*)w->state;
    char interface_str[8] = { 0 };
    sprintf(interface_str, "%d", state->interface);
    char* exec = search_executable("dhcp");
    if (exec != NULL) {
        const char* dhcp_argv[] = { "dhcp", "-i", interface_str, NULL };
        spawn(exec, dhcp_argv, NULL);
        free(exec);
    }
    netinfo_refresh(state);
}

void netinfo_init(window_instance_t* w) {
    netinfo_state_t* state = malloc(sizeof(netinfo_state_t));
    state->interface = 0;
    netinfo_refresh(state);

    button_init(&state->up_btn, w->width - 28, 2, 24, 20, "^", on_netinfo_up, NULL);
    button_init(&state->down_btn, w->width - 28, w->height - TITLE_BAR_HEIGHT - 24, 24, 20, "v", on_netinfo_down, NULL);
    button_init(&state->dhcp_btn, w->width - 54, 26, 50, 20, "DHCP", on_netinfo_dhcp, NULL);
    state->dhcp_btn.bg_color = 0x336644;
    state->dhcp_btn.hover_color = 0x44aa66;

    w->state = state;
    w->title_bar_color = 0x1a3a2e;
    w->is_dirty = true;
}

void netinfo_update(window_instance_t* w, event_t* event) {
    netinfo_state_t* state = (netinfo_state_t*)w->state;

    w->is_dirty = true;

    button_handle_event(&state->up_btn, w, event);
    button_handle_event(&state->down_btn, w, event);
    button_handle_event(&state->dhcp_btn, w, event);
}

void netinfo_draw(window_instance_t* w) {
    netinfo_state_t* state = (netinfo_state_t*)w->state;

    button_draw(&state->up_btn, w);
    button_draw(&state->down_btn, w);
    button_draw(&state->dhcp_btn, w);

    char buffer[128] = { 0 };

    if (!state->nic_valid) {
        sprintf(buffer, "no nic %d", state->interface);
        window_draw_string(w, 4, 4, buffer, 0xff4444);
        return;
    }

    nic_content_t* nic = &state->nic_data;

    int line = 0;
    sprintf(buffer, "Interface: %d", state->interface);
    window_draw_string(w, 4, line * 16 + 4, buffer, 0xaaffcc); line += 2;

    sprintf(buffer, "ip:   %d.%d.%d.%d", nic->ip_config.ip.ip_p[0], nic->ip_config.ip.ip_p[1], nic->ip_config.ip.ip_p[2], nic->ip_config.ip.ip_p[3]);
    window_draw_string(w, 4, line * 16 + 4, buffer, 0xffffff); line++;

    sprintf(buffer, "mask: %d.%d.%d.%d", nic->ip_config.subnet_mask.ip_p[0], nic->ip_config.subnet_mask.ip_p[1], nic->ip_config.subnet_mask.ip_p[2], nic->ip_config.subnet_mask.ip_p[3]);
    window_draw_string(w, 4, line * 16 + 4, buffer, 0xffffff); line++;

    sprintf(buffer, "gw:   %d.%d.%d.%d", nic->ip_config.gateway_ip.ip_p[0], nic->ip_config.gateway_ip.ip_p[1], nic->ip_config.gateway_ip.ip_p[2], nic->ip_config.gateway_ip.ip_p[3]);
    window_draw_string(w, 4, line * 16 + 4, buffer, 0xffffff); line++;

    sprintf(buffer, "dns:  %d.%d.%d.%d", nic->ip_config.dns_ip.ip_p[0], nic->ip_config.dns_ip.ip_p[1], nic->ip_config.dns_ip.ip_p[2], nic->ip_config.dns_ip.ip_p[3]);
    window_draw_string(w, 4, line * 16 + 4, buffer, 0xffffff); line++;

    sprintf(buffer, "mac:  %x:%x:%x:%x:%x:%x", nic->mac.mac_p[0], nic->mac.mac_p[1], nic->mac.mac_p[2], nic->mac.mac_p[3], nic->mac.mac_p[4], nic->mac.mac_p[5]);
    window_draw_string(w, 4, line * 16 + 4, buffer, 0xffffff); line++;

    sprintf(buffer, "dev:  %s", nic->name);
    window_draw_string(w, 4, line * 16 + 4, buffer, 0xffffff);
}

void netinfo_cleanup(window_instance_t* w) {
    if (w->state) {
        free(w->state);
        w->state = NULL;
    }
}

window_definition_t netinfo_definition = {
    .name = "Network Info",
    .register_window = register_netinfo_window,
};

void register_netinfo_window(void) {
    window_add(80, 80, 280, 250, "Network Info", 0x051015, netinfo_init, netinfo_update, netinfo_draw, netinfo_cleanup);
}
