#pragma once

#include <fs/devfs.h>
#include <stdint.h>

typedef struct fb_info {
    uint32_t fb_pitch;
    uint32_t fb_width;
    uint32_t fb_height;
    uint8_t fb_bpp;
    uint32_t fb_addr;
} fb_info_t;

extern devfs_file_t framebuffer_file;