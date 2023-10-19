#pragma once

#include <sys/graphics.h>

fb_info_t fb_load_info();


#ifdef FB_SET_PX_IMPL
static inline void fb_set_pixel(fb_info_t* info, uint32_t x, uint32_t y, uint32_t color) {
#ifndef FB_UNSAFE_SETPX
    if (x >= info->fb_width || y >= info->fb_height || x < 0 || y < 0) {
        return;
    }
#endif
	*(uint32_t*) (info->fb_addr + (x * 4) + (y * info->fb_pitch)) = color;
}
#endif