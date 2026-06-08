#include "DOOM/DOOM.h"
#include "DOOM/doomdef.h"
#define DOOM_IMPLEMENTATION 
// #include "PureDOOM.h"

#include <stdint.h>
#include <string.h>

#include <non-standard/sys/env.h>

#include <wm_client.h>
#include <wm_protocol.h>

#include <stdio.h>
#include <stdlib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static int currentTick = 0;

void emulate_doom_gettime(int* sec, int* usec) {
    *sec = currentTick / TICRATE;
    *usec = (currentTick % TICRATE) * (1000000 / TICRATE);
}

int main(int argc, char* argv[]) {
    wm_client_t client;
    wm_client_init(&client);

    wm_client_set_title(&client, "DOOM");
    wm_client_set_title_bar_color(&client, 0x1a3a2e);
    wm_client_set_bg_color(&client, 0xffffffff);

    doom_set_gettime(emulate_doom_gettime);

    doom_set_default_int("key_up", DOOM_KEY_W);
    doom_set_default_int("key_down", DOOM_KEY_S);
    doom_set_default_int("key_strafeleft", DOOM_KEY_A);
    doom_set_default_int("key_straferight", DOOM_KEY_D);
    doom_set_default_int("key_use", DOOM_KEY_E);
    doom_set_default_int("key_fire", 'f');
    doom_set_default_int("mouse_move", 0);

    
    int SCALE = 1;


    doom_init(argc, argv, 0);


    int next_key_up = 0;
    
    while (!wm_client_should_close(&client)) {
        int w = wm_client_width(&client);
        int h = wm_client_height(&client);
        
        SCALE = MIN(w / 320, h / 200);
        if (SCALE < 1) {
            SCALE = 1;
        }

       if (next_key_up) {
            doom_key_up(next_key_up);
            next_key_up = 0;
       }


        wm_event_t evt;
        while (wm_client_poll_event(&client, &evt)) {
            int key = 0;
            if (evt.type == WM_EVENT_RESIZE) {
                continue;
            } else if (evt.type == WM_EVENT_KEY_PRESS) {
                key = evt.key;
            } else if (evt.type == WM_EVENT_ARROW_KEY) {
                switch (evt.arrow) {
                    case WM_ARROW_UP:
                        key = DOOM_KEY_UP_ARROW;
                        break;
                    case WM_ARROW_DOWN:
                        key = DOOM_KEY_DOWN_ARROW;
                        break;
                    case WM_ARROW_LEFT:
                        key = DOOM_KEY_LEFT_ARROW;
                        break;
                    case WM_ARROW_RIGHT:
                        key = DOOM_KEY_RIGHT_ARROW;
                        break;
                }
            }

            if (key) {
                doom_key_down(key);
                next_key_up = key;
                break;
            }
        }

        doom_update();

        const uint32_t* framebuffer = (uint32_t*) doom_get_framebuffer(4);
            
        for (int y = 0; y < 200 * SCALE; y++) {
            for (int x = 0; x < 320 * SCALE; x++) {
                int source = framebuffer[(x / SCALE) + (y / SCALE) * 320]; // RGBA
                int target = 0; // ARGB
                target |= (source & 0xFF000000); // A
                target |= (source & 0x00FF0000) >> 16; // R
                target |= (source & 0x0000FF00); // G
                target |= (source & 0x000000FF) << 16; // B

                wm_client_set_pixel(&client, x, y, target);
            }
        }

        wm_client_flush(&client);

        currentTick++;
    }

    return 0;
}