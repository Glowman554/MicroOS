#include <wm_client.h>
#include <wm_protocol.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/file.h>
#include <non-standard/net/ipv4.h>
#include <non-standard/buildin/path.h>
#include <stdlib.h>
#include <stdio.h>

#define BG_COLOR 0x051015

int nic_valid(int interface) {
    char buffer[16] = { 0 };
    sprintf(buffer, "dev:nic%d", interface);

    int fd = open(buffer, FILE_OPEN_MODE_READ);
    if (fd < 0) {
        return 0;
    }

    close(fd);
    return 1;
}

int main(int argc, char** argv) {
    wm_client_t client;
    wm_client_init(&client);

    wm_client_set_title(&client, "Network Info");
    wm_client_set_title_bar_color(&client, 0x1a3a2e);
    wm_client_set_bg_color(&client, BG_COLOR);

    int cw = wm_client_width(&client);
    int ch = wm_client_height(&client);
    int interface = 0;

    wm_button_t up_btn, down_btn, dhcp_btn;
    wm_btn_init(&up_btn, cw - 28, 2, 24, 20, "^");
    wm_btn_init(&down_btn, cw - 28, ch - 24, 24, 20, "v");
    wm_btn_init(&dhcp_btn, cw - 54, 26, 50, 20, "DHCP");
    dhcp_btn.bg_color = 0x336644;
    dhcp_btn.hover_color = 0x44aa66;

    int need_redraw = 1;

    while (!wm_client_should_close(&client)) {
        wm_event_t evt;
        while (wm_client_poll_event(&client, &evt)) {
            if (evt.type == WM_EVENT_MOUSE_CLICK && evt.button == WM_MOUSE_BUTTON_LEFT) {
                if (wm_btn_hit(&up_btn, evt.x, evt.y)) {
                    interface--;
                    need_redraw = 1;
                }

                if (wm_btn_hit(&down_btn, evt.x, evt.y)) {
                    interface++;
                    need_redraw = 1;
                }

                if (wm_btn_hit(&dhcp_btn, evt.x, evt.y)) {
                    char cmd_str[32] = { 0 };
                    sprintf(cmd_str, "dhcp -i %d", interface);
                    system(cmd_str);
                    need_redraw = 1;
                }
            }

            if (evt.type == WM_EVENT_MOUSE_MOVE) {
                int changed = 0;
                changed |= wm_btn_update_hover(&up_btn, evt.x, evt.y);
                changed |= wm_btn_update_hover(&down_btn, evt.x, evt.y);
                changed |= wm_btn_update_hover(&dhcp_btn, evt.x, evt.y);
                if (changed) {
                    need_redraw = 1;
                }
            }
        }


        if (need_redraw) {
            int w = wm_client_width(&client);
            int h = wm_client_height(&client);
            wm_client_fill_rect(&client, 0, 0, w, h, BG_COLOR);

            wm_btn_draw(&up_btn, &client);
            wm_btn_draw(&down_btn, &client);
            wm_btn_draw(&dhcp_btn, &client);

            char buf[128] = { 0 };
            int line = 0;
            sprintf(buf, "Interface: %d", interface);
            wm_client_draw_string(&client, 4, line * 16 + 4, buf, 0xaaffcc, BG_COLOR);
            line += 2;

            if (!nic_valid(interface)) {
                sprintf(buf, "no nic %d", interface);
                wm_client_draw_string(&client, 4, line * 16 + 4, buf, 0xff4444, BG_COLOR);
            } else {
                nic_content_t nic = nic_read(interface);

                sprintf(buf, "ip:   %d.%d.%d.%d", nic.ip_config.ip.ip_p[0], nic.ip_config.ip.ip_p[1], nic.ip_config.ip.ip_p[2], nic.ip_config.ip.ip_p[3]);
                wm_client_draw_string(&client, 4, line * 16 + 4, buf, 0xffffff, BG_COLOR); line++;

                sprintf(buf, "mask: %d.%d.%d.%d", nic.ip_config.subnet_mask.ip_p[0], nic.ip_config.subnet_mask.ip_p[1], nic.ip_config.subnet_mask.ip_p[2], nic.ip_config.subnet_mask.ip_p[3]);
                wm_client_draw_string(&client, 4, line * 16 + 4, buf, 0xffffff, BG_COLOR); line++;

                sprintf(buf, "gw:   %d.%d.%d.%d", nic.ip_config.gateway_ip.ip_p[0], nic.ip_config.gateway_ip.ip_p[1], nic.ip_config.gateway_ip.ip_p[2], nic.ip_config.gateway_ip.ip_p[3]);
                wm_client_draw_string(&client, 4, line * 16 + 4, buf, 0xffffff, BG_COLOR); line++;

                sprintf(buf, "dns:  %d.%d.%d.%d", nic.ip_config.dns_ip.ip_p[0], nic.ip_config.dns_ip.ip_p[1], nic.ip_config.dns_ip.ip_p[2], nic.ip_config.dns_ip.ip_p[3]);
                wm_client_draw_string(&client, 4, line * 16 + 4, buf, 0xffffff, BG_COLOR); line++;

                sprintf(buf, "mac:  %x:%x:%x:%x:%x:%x", nic.mac.mac_p[0], nic.mac.mac_p[1], nic.mac.mac_p[2], nic.mac.mac_p[3], nic.mac.mac_p[4], nic.mac.mac_p[5]);
                wm_client_draw_string(&client, 4, line * 16 + 4, buf, 0xffffff, BG_COLOR); line++;

                sprintf(buf, "dev:  %s", nic.name);
                wm_client_draw_string(&client, 4, line * 16 + 4, buf, 0xffffff, BG_COLOR);
            }

            wm_client_flush(&client);
            need_redraw = 0;
        }

        yield();
    }

    return 0;
}
