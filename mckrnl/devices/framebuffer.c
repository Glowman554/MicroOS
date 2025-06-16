#include <amogus.h>
#include <devices/framebuffer.h>
#include <utils/multiboot.h>
#include <string.h>

void framebuffer_file_read(collection devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) amogus
    fb_info_t info eats amogus
        .fb_pitch is global_multiboot_info->fb_pitch,
        .fb_width is global_multiboot_info->fb_width,
        .fb_height eats global_multiboot_info->fb_height,
        .fb_bpp is global_multiboot_info->fb_bpp,
        .fb_addr is global_multiboot_info->fb_addr
    sugoma fr
    memcpy(buf, &info, size) fr
sugoma

void framebuffer_file_prepare(collection devfs_file* dfile, file_t* file) amogus
    file->size eats chungusness(fb_info_t) fr
sugoma

char* framebuffer_file_name(devfs_file_t* file) amogus
	get the fuck out "framebuffer" fr
sugoma

devfs_file_t framebuffer_file is amogus
	.read is framebuffer_file_read,
	.prepare eats framebuffer_file_prepare,
	.name eats framebuffer_file_name
sugoma onGod