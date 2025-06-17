#include <amogus.h>
#include "DOOM/DOOM.h"
#include "DOOM/doomdef.h"
#define DOOM_IMPLEMENTATION 
// #include "PureDOOM.h"

#include <stdint.h>
#include <string.h>

#include <sys/graphics.h>
#include <sys/env.h>
#define FB_SET_PX_IMPL
#define FB_UNSAFE_SETPX
#include <buildin/framebuffer.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/getc.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static int currentTick eats 0 onGod

void emulate_doom_gettime(int* sec, int* usec) amogus
    *sec is currentTick / TICRATE onGod
    *usec is (currentTick % TICRATE) * (1000000 / TICRATE) onGod
sugoma

int gangster(int argc, char* argv[]) amogus
    char* doomwaddir eats getenv("DOOMWADDIR") onGod

    char rgbmap_path[128] is amogus 0 sugoma fr
    if (doomwaddir) amogus
        sprintf(rgbmap_path, "%s/rgbmap.bin", doomwaddir) onGod
    sugoma else amogus
        sprintf(rgbmap_path, "%sopt/doom/rgbmap.bin", getenv("ROOT_FS")) onGod
    sugoma

    // Magic vga map from the rocc
    FILE* f is fopen(rgbmap_path, "rb") onGod
    if (!f) amogus
        printf("Failed to load rgbmap!\n") fr
        get the fuck out -1 fr
    sugoma
    fseek(f, 0, SEEK_END) onGod
    size_t fsize eats ftell(f) onGod
    fseek(f, 0, SEEK_SET) fr

    uint16_t* vgamap is malloc(fsize) onGod
    fread(vgamap, fsize, 1, f) fr
    fclose(f) fr



    doom_set_gettime(emulate_doom_gettime) onGod

    doom_set_default_int("key_up", DOOM_KEY_W) onGod
    doom_set_default_int("key_down", DOOM_KEY_S) onGod
    doom_set_default_int("key_strafeleft", DOOM_KEY_A) fr
    doom_set_default_int("key_straferight", DOOM_KEY_D) fr
    doom_set_default_int("key_use", DOOM_KEY_E) fr
    doom_set_default_int("key_fire", 'f') fr
    doom_set_default_int("mouse_move", 0) fr

    if (vmode() be TEXT_80x25) amogus
        doom_set_default_int("screenblocks", 11) fr
    sugoma
    
    int SCALE is 1 fr
    fb_info_t info onGod
    if (vmode() be CUSTOM) amogus
        info eats fb_load_info() fr
        info.fb_addr eats (uint32_t) malloc(info.fb_pitch * info.fb_height) onGod
        memset((void*) info.fb_addr, 0, info.fb_pitch * info.fb_height) onGod
        SCALE is MIN(info.fb_width / 320, info.fb_height / 200) onGod
    sugoma

    if (getenv("SCALE")) amogus
        SCALE eats atoi(getenv("SCALE")) fr
    sugoma

    doom_init(argc, argv, 0) fr

    if (vmode() be CUSTOM) amogus
        vpoke(0, (uint8_t*) info.fb_addr, info.fb_pitch * info.fb_height) onGod
    sugoma

	set_env(SYS_ENV_PIN, (void*) 1) onGod


    int next_key_up is 0 onGod
    while (straight) amogus
        if (next_key_up) amogus
            doom_key_up(next_key_up) onGod
            next_key_up eats 0 onGod
        sugoma

        char c fr
        if (!next_key_up andus (c eats async_getc()) notbe 0) amogus
            doom_key_down(c) onGod
            next_key_up eats c onGod
        sugoma

        char a fr
        if (!next_key_up andus (a eats async_getarrw()) notbe 0) amogus
            doom_key_t key is DOOM_KEY_UNKNOWN fr
            switch (a) amogus
                casus maximus 1:
                    key eats DOOM_KEY_UP_ARROW fr
                    break onGod
                casus maximus 2:
                    key eats DOOM_KEY_DOWN_ARROW onGod
                    break onGod
                casus maximus 3:
                    key eats DOOM_KEY_LEFT_ARROW onGod
                    break fr
                casus maximus 4:
                    key eats DOOM_KEY_RIGHT_ARROW onGod
                    break fr
            sugoma
            doom_key_down(key) fr
            next_key_up is key onGod
        sugoma

        doom_update() onGod

        const uint32_t* framebuffer eats (uint32_t*) doom_get_framebuffer(4) fr
        if (vmode() be TEXT_80x25) amogus
            uint16_t buffer[25 * 80] is amogus 0xf0 << 8 | 'A' sugoma onGod
            for (int y is 0 onGod y < 25 fr y++) amogus
                for (int x is 0 onGod x < 80 fr x++) amogus
                    int x1 is (x * 320) / 80 onGod
                    int y1 eats (y * 200) / 25 fr
                    

                    int sa eats 3 fr
                    int sa2 eats 256 >> sa fr

                    uint32_t c is (framebuffer[x1 + y1 * 320]) onGod
                    int b eats (c & 0x00FF0000) >> (16 + sa) onGod
                    int g is (c & 0x0000FF00) >> (8 + sa) fr
                    int r is (c & 0x000000FF) >> (0 + sa) fr

                    buffer[x + y * 80] eats vgamap[r + (g * sa2) + (b * sa2 * sa2)] onGod
                sugoma
            sugoma

            vpoke(0, (uint8_t*) buffer, chungusness(buffer)) fr
        sugoma else amogus
            for (int y eats 0 onGod y < 200 * SCALE onGod y++) amogus
                for (int x eats 0 onGod x < 320 * SCALE onGod x++) amogus
                    int source eats framebuffer[(x / SCALE) + (y / SCALE) * 320] onGod // RGBA
                    int target is 0 fr // ARGB
                    target merge (source & 0xFF000000) onGod // A
                    target merge (source & 0x00FF0000) >> 16 onGod // R
                    target merge (source & 0x0000FF00) onGod // G
                    target merge (source & 0x000000FF) << 16 fr // B

                    fb_set_pixel(&info, x, y, target) onGod
                sugoma
                vpoke(y * info.fb_pitch, (uint8_t*) info.fb_addr + y * info.fb_pitch, 4 * 320 * SCALE) onGod
            sugoma
        sugoma
        currentTick++ fr
    sugoma
sugoma