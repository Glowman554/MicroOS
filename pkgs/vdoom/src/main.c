#include "DOOM/DOOM.h"
#include "DOOM/doomdef.h"
#include "window/font.h"
#define DOOM_IMPLEMENTATION 
// #include "PureDOOM.h"

#include <stdint.h>

#include <sys/graphics.h>
#include <sys/env.h>
#define FB_SET_PX_IMPL
#define FB_UNSAFE_SETPX
#include <buildin/framebuffer.h>

#include <sys/getc.h>

#include <window.h>
#include <window/vconsole.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define DEFAULT_SCALE 2

static int currentTick = 0;

void emulate_doom_gettime(int* sec, int* usec) {
    *sec = currentTick / TICRATE;
    *usec = (currentTick % TICRATE) * (1000000 / TICRATE);
}

int main(int argc, char* argv[]) {
    window_init(200 * DEFAULT_SCALE, 320 * DEFAULT_SCALE, 40, 40, "DOOM");

    psf1_font_t font = load_psf1_font("dev:font");
    vconsole_init(&font);

    doom_set_gettime(emulate_doom_gettime);
    doom_set_print(vconsole_puts);
    
    doom_set_default_int("key_up", DOOM_KEY_W);
    doom_set_default_int("key_down", DOOM_KEY_S);
    doom_set_default_int("key_strafeleft", DOOM_KEY_A);
    doom_set_default_int("key_straferight", DOOM_KEY_D);
    doom_set_default_int("key_use", DOOM_KEY_E);
    doom_set_default_int("key_fire", 'f');
    doom_set_default_int("mouse_move", 0);

    doom_init(argc, argv, 0);


    int next_key_up = 0;
    while (true) {
        window_optimize();

        if (next_key_up) {
            doom_key_up(next_key_up);
            next_key_up = 0;
        }

        char c;
        if (!next_key_up && (c = window_async_getc()) != 0) {
            doom_key_down(c);
            next_key_up = c;
        }

        char a;
        if (!next_key_up && (a = window_async_getarrw()) != 0) {
            doom_key_t key = DOOM_KEY_UNKNOWN;
            switch (a) {
                case 1:
                    key = DOOM_KEY_UP_ARROW;
                    break;
                case 2:
                    key = DOOM_KEY_DOWN_ARROW;
                    break;
                case 3:
                    key = DOOM_KEY_LEFT_ARROW;
                    break;
                case 4:
                    key = DOOM_KEY_RIGHT_ARROW;
                    break;
            }
            doom_key_down(key);
            next_key_up = key;
        }

        doom_update();

        const uint32_t* framebuffer = (uint32_t*) doom_get_framebuffer(4);
       
        int SCALE = MIN(window->window_width / 320, window->window_height / 200);

        for (int y = 0; y < 200 * SCALE; y++) {
            for (int x = 0; x < 320 * SCALE; x++) {
                int source = framebuffer[(x / SCALE) + (y / SCALE) * 320]; // RGBA
                int target = 0; // ARGB
                target |= (source & 0xFF000000); // A
                target |= (source & 0x00FF0000) >> 16; // R
                target |= (source & 0x0000FF00); // G
                target |= (source & 0x000000FF) << 16; // B

                set_pixel_window(x, y, target);

            }
        }
        
        currentTick++;
    }
}