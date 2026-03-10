#include "netinfo.h"
#include <window.h>
#include <window_helpers.h>
#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/spawn.h>
#include <buildin/path.h>
#include <net/ipv4.h>

extern psf1_font_t font;
extern fpic_image_t up_arrow;
extern fpic_image_t down_arrow;
extern fpic_image_t dhcp_button;

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

void netinfo_init(window_instance_t* w) {
    netinfo_state_t* state = malloc(sizeof(netinfo_state_t));
    state->interface = 0;
    netinfo_refresh(state);

    state->up_area.x      = w->width - 16;
    state->up_area.y      = TITLE_BAR_HEIGHT;
    state->up_area.width  = 16;
    state->up_area.height = 16;

    state->down_area.x      = w->width - 16;
    state->down_area.y      = w->height - 16;
    state->down_area.width  = 16;
    state->down_area.height = 16;

    state->dhcp_area.x      = w->width - 16;
    state->dhcp_area.y      = TITLE_BAR_HEIGHT + 32;
    state->dhcp_area.width  = 16;
    state->dhcp_area.height = 16;

    w->state = state;
    w->title_bar_color = 0x1a3a2e;
    w->is_dirty = true;
}

void netinfo_update(window_instance_t* w, event_t* event) {
    netinfo_state_t* state = (netinfo_state_t*)w->state;

    if (event->type == EVENT_MOUSE_CLICK && event->button == MOUSE_BUTTON_LEFT) {
        click_area_t* up   = &state->up_area;
        click_area_t* dn   = &state->down_area;
        click_area_t* dhcp = &state->dhcp_area;

        if (event->x >= up->x && event->x < up->x + up->width &&
            event->y >= up->y && event->y < up->y + up->height) {
            state->interface--;
            netinfo_refresh(state);
            w->is_dirty = true;
        }

        if (event->x >= dn->x && event->x < dn->x + dn->width &&
            event->y >= dn->y && event->y < dn->y + dn->height) {
            state->interface++;
            netinfo_refresh(state);
            w->is_dirty = true;
        }

        if (event->x >= dhcp->x && event->x < dhcp->x + dhcp->width &&
            event->y >= dhcp->y && event->y < dhcp->y + dhcp->height) {
            char interface_str[8] = { 0 };
            sprintf(interface_str, "%d", state->interface);

            char* exec = search_executable("dhcp");
            if (exec != NULL) {
                const char* dhcp_argv[] = { "dhcp", "-i", interface_str, NULL };
                spawn(exec, dhcp_argv, NULL);
                free(exec);
            }
            netinfo_refresh(state);
            w->is_dirty = true;
        }
    }
}

void netinfo_draw(window_instance_t* w) {
    netinfo_state_t* state = (netinfo_state_t*)w->state;

    for (int x = 0; x < w->width; x++) {
        for (int y = TITLE_BAR_HEIGHT; y < w->height; y++) {
            window_set_pixel(w, x, y, 0x051015);
        }
    }

    click_area_t* up   = &state->up_area;
    click_area_t* dn   = &state->down_area;
    click_area_t* dhcp = &state->dhcp_area;

    window_draw_fpic(w, &up_arrow,    up->x,   up->y);
    window_draw_fpic(w, &down_arrow,  dn->x,   dn->y);
    window_draw_fpic(w, &dhcp_button, dhcp->x, dhcp->y);

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
