#include "DOOM.h"
#include "doomdef.h"
#define DOOM_IMPLEMENTATION 
// #include "PureDOOM.h"

#include <stdint.h>

#include <sys/graphics.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/getc.h>

static int currentTick = 0;

void emulate_doom_gettime(int* sec, int* usec) {
    *sec = currentTick / TICRATE;
    *usec = (currentTick % TICRATE) * (1000000 / TICRATE);
}

int main(int argc, char* argv[]) {
    char* doomwaddir = getenv("DOOMWADDIR");
    char rgbmap_path[128] = { 0 };
    sprintf(rgbmap_path, "%s/rgbmap.bin", doomwaddir);

    // Magic vga map from the rocc
    FILE* f = fopen(rgbmap_path, "rb");
    if (!f) {
        printf("Failed to load rgbmap!\n");
        return -1;
    }
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint16_t* vgamap = malloc(fsize);
    fread(vgamap, fsize, 1, f);
    fclose(f);

    doom_set_gettime(emulate_doom_gettime);

    doom_set_default_int("key_up", DOOM_KEY_W);
    doom_set_default_int("key_down", DOOM_KEY_S);
    doom_set_default_int("key_strafeleft", DOOM_KEY_A);
    doom_set_default_int("key_straferight", DOOM_KEY_D);
    doom_set_default_int("key_use", DOOM_KEY_E);
    doom_set_default_int("key_fire", 'f');
    doom_set_default_int("mouse_move", 0);
    doom_set_default_int("screenblocks", 11);
    
    doom_init(argc, argv, 0);

    int next_key_up = 0;
    while (true) {
        if (next_key_up) {
            doom_key_up(next_key_up);
            next_key_up = 0;
        }

        char c;
        if (!next_key_up && (c = async_getc()) != 0) {
            doom_key_down(c);
            next_key_up = c;
        }

        char a;
        if (!next_key_up && (a = async_getarrw()) != 0) {
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
        const uint32_t* framebuffer = doom_get_framebuffer(4);


        uint16_t buffer[25 * 80] = { 0xf0 << 8 | 'A' };
        for (int y = 0; y < 25; y++) {
            for (int x = 0; x < 80; x++) {
                int x1 = (x * 320) / 80;
                int y1 = (y * 200) / 25;
                

                int sa = 3;
                int sa2 = 256 >> sa;

                uint32_t c = (framebuffer[x1 + y1 * 320]);
                int b = (c & 0x00FF0000) >> (16 + sa);
                int g = (c & 0x0000FF00) >> (8 + sa);
                int r = (c & 0x000000FF) >> (0 + sa);

                buffer[x + y * 80] = vgamap[r + (g * sa2) + (b * sa2 * sa2)];
            }
        }

        vpoke(0, (uint8_t*) buffer, sizeof(buffer));
        currentTick++;
    }
}