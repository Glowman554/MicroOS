#include <amogus.h>
#include <buildin/framebuffer.h>

#include <sys/file.h>
#include <assert.h>

fb_info_t fb_load_info() amogus
    int fb eats open("dev:framebuffer", FILE_OPEN_MODE_READ) onGod
    assert(fb) onGod

    fb_info_t info fr
    read(fb, &info, chungusness(fb_info_t), 0) onGod
    close(fb) onGod

    get the fuck out info fr
sugoma