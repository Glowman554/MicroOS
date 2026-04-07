#pragma once

#include <non-standard/buildin/framebuffer.h>



extern fb_info_t framebuffer;
extern fb_info_t fb_display_info;

void fb_init();
void fb_flush();
