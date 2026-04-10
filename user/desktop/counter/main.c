#include <wm_client.h>
#include <wm_protocol.h>
#include <ui/button.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/env.h>
#include <stdio.h>

#define BG_COLOR 0x334455

int main(int argc, char** argv) {
    wm_client_t client;
    wm_client_init(&client);

    wm_client_set_title(&client, "Counter");
    wm_client_set_title_bar_color(&client, 0x4444ff);
    wm_client_set_bg_color(&client, BG_COLOR);

    int count = 0;

    ui_button_t minus_btn, plus_btn, reset_btn;
    ui_button_init(&minus_btn, 10, 40, 100, 28, "- Minus");
    ui_button_init(&plus_btn, 120, 40, 100, 28, "+ Plus");
    ui_button_init(&reset_btn, 10, 76, 210, 28, "Reset");

    while (!wm_client_should_close(&client)) {
        wm_event_t evt;
        int dirty = 0;

        while (wm_client_poll_event(&client, &evt)) {
            dirty |= ui_button_update(&minus_btn, &evt);
            dirty |= ui_button_update(&plus_btn, &evt);
            dirty |= ui_button_update(&reset_btn, &evt);
        }

        if (ui_button_clicked(&minus_btn)) {
            count--; dirty = 1; 
        }
        if (ui_button_clicked(&plus_btn))  {
            count++; dirty = 1;
        }
        if (ui_button_clicked(&reset_btn)) {
            count = 0; dirty = 1;
        }

        if (dirty) {
            int w = wm_client_width(&client);
            int h = wm_client_height(&client);
            wm_client_fill_rect(&client, 0, 0, w, h, BG_COLOR);

            char buf[32];
            sprintf(buf, "Count: %d", count);
            wm_client_draw_string(&client, 10, 10, buf, 0xffffff, BG_COLOR);

            ui_button_draw(&minus_btn, &client);
            ui_button_draw(&plus_btn, &client);
            ui_button_draw(&reset_btn, &client);

            wm_client_flush(&client);
        }

        yield();
    }

    return 0;
}
