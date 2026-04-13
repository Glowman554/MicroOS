#include <wm_client.h>
#include <wm_protocol.h>
#include <ui/button.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/time.h>
#include <non-standard/sys/raminfo.h>
#include <non-standard/sys/env.h>
#include <non-standard/buildin/unix_time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BG_COLOR 0x1a0a2e

#define is_kb(x) ((x) >= 1024)
#define is_mb(x) ((x) >= 1024 * 1024)
#define is_gb(x) ((unsigned)(x) >= 1024u * 1024u * 1024u)
#define to_kb(x) ((x) / 1024)
#define to_mb(x) ((x) / 1024 / 1024)
#define to_gb(x) ((x) / 1024 / 1024 / 1024)

void format_memory_usage(char* out_buf, uint32_t usage) {
    if (is_gb(usage)) {
        sprintf(out_buf, "%d,%d GB", to_gb(usage), to_mb(usage) % 1024);
    } else if (is_mb(usage)) {
        sprintf(out_buf, "%d,%d MB", to_mb(usage), to_kb(usage) % 1024);
    } else if (is_kb(usage)) {
        sprintf(out_buf, "%d,%d KB", to_kb(usage), usage % 1024);
    } else {
        sprintf(out_buf, "%d B", usage);
    }
}

int main(int argc, char** argv) {
    wm_client_t client;
    wm_client_init(&client);

    wm_client_set_title(&client, "System Control");
    wm_client_set_title_bar_color(&client, 0x553366);
    wm_client_set_bg_color(&client, BG_COLOR);

    int cw = wm_client_width(&client);

    ui_button_t reboot_btn, shutdown_btn;
    ui_button_init(&reboot_btn, cw - 80, 2, 76, 22, "Reboot");
    reboot_btn.bg_color = 0x554400;
    reboot_btn.hover_color = 0x887700;
    ui_button_init(&shutdown_btn, cw - 80, 28, 76, 22, "Shutdown");
    shutdown_btn.bg_color = 0x660000;
    shutdown_btn.hover_color = 0xaa2222;

    while (!wm_client_should_close(&client)) {
        wm_event_t evt;
        while (wm_client_poll_event(&client, &evt)) {
            ui_button_update(&reboot_btn, &evt);
            ui_button_update(&shutdown_btn, &evt);
        }

        if (ui_button_clicked(&reboot_btn)) {
            env(SYS_PWR_RESET_ID);
        }
        if (ui_button_clicked(&shutdown_btn)) {
            env(SYS_PWR_SHUTDOWN_ID);
        }

        int w = wm_client_width(&client);
        int h = wm_client_height(&client);
        wm_client_fill_rect(&client, 0, 0, w, h, BG_COLOR);

        ui_button_draw(&reboot_btn, &client);
        ui_button_draw(&shutdown_btn, &client);

        char timebuf[128] = { 0 };
        unix_time_to_string(time(NULL), timebuf);

        char timemsbuf[32] = { 0 };
        sprintf(timemsbuf, "%d ms", time_ms());

        uint32_t mem_free, mem_used;
        raminfo(&mem_free, &mem_used);

        char total_str[32] = { 0 };
        char free_str[32]  = { 0 };
        char used_str[32]  = { 0 };
        format_memory_usage(total_str, mem_free + mem_used);
        format_memory_usage(free_str,  mem_free);
        format_memory_usage(used_str,  mem_used);

        int line = 0;
        wm_client_draw_string(&client, 4, line * 16 + 4, "TIME:",   0xaaaaff, BG_COLOR); line++;
        wm_client_draw_string(&client, 4, line * 16 + 4, timebuf,   0xffffff, BG_COLOR); line++;
        line++;
        wm_client_draw_string(&client, 4, line * 16 + 4, "TIMEMS:", 0xaaaaff, BG_COLOR); line++;
        wm_client_draw_string(&client, 4, line * 16 + 4, timemsbuf, 0xffffff, BG_COLOR); line++;
        line++;
        wm_client_draw_string(&client, 4, line * 16 + 4, "TOTAL:",  0xaaaaff, BG_COLOR); line++;
        wm_client_draw_string(&client, 4, line * 16 + 4, total_str, 0xffffff, BG_COLOR); line++;
        wm_client_draw_string(&client, 4, line * 16 + 4, "FREE:",   0xaaaaff, BG_COLOR); line++;
        wm_client_draw_string(&client, 4, line * 16 + 4, free_str,  0xffffff, BG_COLOR); line++;
        wm_client_draw_string(&client, 4, line * 16 + 4, "USED:",   0xaaaaff, BG_COLOR); line++;
        wm_client_draw_string(&client, 4, line * 16 + 4, used_str,  0xffffff, BG_COLOR);

        wm_client_flush(&client);
        yield();
    }

    return 0;
}
