#include <wm_client.h>
#include <wm_protocol.h>
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

    wm_button_t minus_btn, plus_btn, reset_btn;
    wm_btn_init(&minus_btn, 10, 40, 100, 28, "- Minus");
    wm_btn_init(&plus_btn, 120, 40, 100, 28, "+ Plus");
    wm_btn_init(&reset_btn, 10, 76, 210, 28, "Reset");

    while (!wm_client_should_close(&client)) {
        wm_event_t evt;
        int dirty = 0;

        while (wm_client_poll_event(&client, &evt)) {
            if (evt.type == WM_EVENT_MOUSE_CLICK && evt.button == WM_MOUSE_BUTTON_LEFT) {
                int rx = evt.x;
                int ry = evt.y;
                if (wm_btn_hit(&minus_btn, rx, ry)) {
                    count--;
                    dirty = 1;
                }
               
                if (wm_btn_hit(&plus_btn, rx, ry)) {
                    count++;
                    dirty = 1;
                }
               
                if (wm_btn_hit(&reset_btn, rx, ry)) {
                    count = 0;
                    dirty = 1;
                }
            }
            if (evt.type == WM_EVENT_MOUSE_MOVE) {
                int rx = evt.x;
                int ry = evt.y;
                dirty |= wm_btn_update_hover(&minus_btn, rx, ry);
                dirty |= wm_btn_update_hover(&plus_btn, rx, ry);
                dirty |= wm_btn_update_hover(&reset_btn, rx, ry);
            }
        }

        if (dirty) {
            int w = wm_client_width(&client);
            int h = wm_client_height(&client);
            wm_client_fill_rect(&client, 0, 0, w, h, BG_COLOR);

            char buf[32];
            sprintf(buf, "Count: %d", count);
            wm_client_draw_string(&client, 10, 10, buf, 0xffffff, BG_COLOR);

            wm_btn_draw(&minus_btn, &client);
            wm_btn_draw(&plus_btn, &client);
            wm_btn_draw(&reset_btn, &client);

            wm_client_flush(&client);
        }

        yield();
    }

    return 0;
}
