#include <window.h>
#include <window/font.h>
#include <window/fpic.h>
#include <window/clickarea.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/spawn.h>
#include <buildin/path.h>
#include <net/ipv4.h>

#define BGCOLOR 0x0
#define FGCOLOR 0xffffffff

psf1_font_t font;
fpic_image_t* up_arrow;
click_area_t up_arrow_area;
fpic_image_t* down_arrow;
click_area_t down_arrow_area;

fpic_image_t* dhcp_button;
click_area_t dhcp_button_area;

int interface = 0;

void draw() {
    for (int i = 0; i < window->window_width; i++) {
        for (int j = 0; j < window->window_height; j++) {
            set_pixel_window(i, j, BGCOLOR);
        }
    }

    up_arrow_area.x = window->window_width - 16;
    up_arrow_area.y = 0;
    up_arrow_area.width = 16;
    up_arrow_area.height = 16;
    draw_fpic_window(up_arrow, up_arrow_area.x, up_arrow_area.y);

    down_arrow_area.x = window->window_width - 16;
    down_arrow_area.y = window->window_height - 16;
    down_arrow_area.width = 16;
    down_arrow_area.height = 16;
    draw_fpic_window(down_arrow, down_arrow_area.x, down_arrow_area.y);

    dhcp_button_area.x = window->window_width - 16;
    dhcp_button_area.y = 32;
    dhcp_button_area.width = 16;
    dhcp_button_area.height = 16;
    draw_fpic_window(dhcp_button, dhcp_button_area.x, dhcp_button_area.y);

    char buffer[128] = { 0 };
    sprintf(buffer, "dev:nic%d", interface);

    int fd = open(buffer, FILE_OPEN_MODE_READ);
    if (fd < 0) {
        sprintf(buffer, "failed to open nic %d", interface);
        draw_string_window(&font, 0, 0, buffer, FGCOLOR, BGCOLOR);
        return;
    }
    close(fd);

    nic_content_t nic_config = nic_read(interface);

    int line = 0;
    sprintf(buffer, "Interface: %d", interface);
    draw_string_window(&font, 0, line * 16, buffer, FGCOLOR, BGCOLOR);
    line += 2;

    sprintf(buffer, "address: %d.%d.%d.%d", nic_config.ip_config.ip.ip_p[0], nic_config.ip_config.ip.ip_p[1], nic_config.ip_config.ip.ip_p[2], nic_config.ip_config.ip.ip_p[3]);
    draw_string_window(&font, 0, line * 16, buffer, FGCOLOR, BGCOLOR);
    line += 1;

    sprintf(buffer, "subnet: %d.%d.%d.%d", nic_config.ip_config.subnet_mask.ip_p[0], nic_config.ip_config.subnet_mask.ip_p[1], nic_config.ip_config.subnet_mask.ip_p[2], nic_config.ip_config.subnet_mask.ip_p[3]);
    draw_string_window(&font, 0, line * 16, buffer, FGCOLOR, BGCOLOR);
    line += 1;

    sprintf(buffer, "gateway: %d.%d.%d.%d", nic_config.ip_config.gateway_ip.ip_p[0], nic_config.ip_config.gateway_ip.ip_p[1], nic_config.ip_config.gateway_ip.ip_p[2], nic_config.ip_config.gateway_ip.ip_p[3]);
    draw_string_window(&font, 0, line * 16, buffer, FGCOLOR, BGCOLOR);
    line += 1;
    
    sprintf(buffer, "dns: %d.%d.%d.%d", nic_config.ip_config.dns_ip.ip_p[0], nic_config.ip_config.dns_ip.ip_p[1], nic_config.ip_config.dns_ip.ip_p[2], nic_config.ip_config.dns_ip.ip_p[3]);
    draw_string_window(&font, 0, line * 16, buffer, FGCOLOR, BGCOLOR);
    line += 1;

    sprintf(buffer, "mac: %x:%x:%x:%x:%x:%x", nic_config.mac.mac_p[0], nic_config.mac.mac_p[1], nic_config.mac.mac_p[2], nic_config.mac.mac_p[3], nic_config.mac.mac_p[4], nic_config.mac.mac_p[5]);
    draw_string_window(&font, 0, line * 16, buffer, FGCOLOR, BGCOLOR);
    line += 1;

    sprintf(buffer, "device: %s",nic_config.name);
    draw_string_window(&font, 0, line * 16, buffer, FGCOLOR, BGCOLOR);
    line += 1;
}

int main(int argc, char* argv[], const char* envp[]) {
    window_init(200, 400, 50, 50, "Network info");

    font = load_psf1_font("dev:font");

    up_arrow = load_fpic_window("icons/programs/explorer/up_arrow.fpic");
    down_arrow = load_fpic_window("icons/programs/explorer/down_arrow.fpic");
    dhcp_button = load_fpic_window("icons/programs/netinfo/dhcp.fpic");

    while (true) {
        window_optimize();

        if (window_redrawn()) {
            draw();
        }

        mouse_info_t info;
        window_mouse_info(&info);

        if (check_click_area_window(&up_arrow_area, &info)) {
            interface--;
            draw();
        }

        if (check_click_area_window(&down_arrow_area, &info)) {
            interface++;
            draw();
        }

        if (check_click_area_window(&dhcp_button_area, &info)) {
            char interface_str[8] = { 0 };
            sprintf(interface_str, "%d", interface);

            char* exec = search_executable("dhcp");
            if (exec == NULL) {
                printf("Could not find dhcp in PATH!\n");
                return -1;
            }
        
            const char* dhcp_argv[] = {
                "dhcp",
                "-i",
                interface_str,
                NULL
            };

            int pid = spawn(exec, dhcp_argv, envp);
            while (get_proc_info(pid)) {
                yield();
            }

    	    free(exec);

            window->dirty = true;
        }
    }
}