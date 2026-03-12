#include <framebuffer.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <non-standart/sys/graphics.h>
#include <non-standart/sys/file.h>

fb_info_t framebuffer;
fb_info_t fb_display_info;

void fb_init() {
    fb_display_info = fb_load_info();
    framebuffer.fb_width = fb_display_info.fb_width;
    framebuffer.fb_height = fb_display_info.fb_height;
    framebuffer.fb_pitch = fb_display_info.fb_pitch;
    framebuffer.fb_bpp = fb_display_info.fb_bpp;
    framebuffer.fb_addr = (uint32_t) malloc(fb_display_info.fb_pitch * fb_display_info.fb_height);
    memset((void*)framebuffer.fb_addr, 0, fb_display_info.fb_pitch * fb_display_info.fb_height);
}

void fb_flush() {
    vpoke(0, (void*)framebuffer.fb_addr, fb_display_info.fb_pitch * fb_display_info.fb_height);
}
