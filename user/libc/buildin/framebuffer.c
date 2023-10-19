#include <buildin/framebuffer.h>

#include <sys/file.h>
#include <assert.h>

fb_info_t fb_load_info() {
    int fb = open("dev:framebuffer", FILE_OPEN_MODE_READ);
    assert(fb);

    fb_info_t info;
    read(fb, &info, sizeof(fb_info_t), 0);
    close(fb);

    return info;
}