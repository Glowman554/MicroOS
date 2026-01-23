#include <devices/framebuffer.h>
#include <utils/multiboot.h>
#include <string.h>

void framebuffer_file_read(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    fb_info_t info = {
        .fb_pitch = global_multiboot_info->fb_pitch,
        .fb_width = global_multiboot_info->fb_width,
        .fb_height = global_multiboot_info->fb_height,
        .fb_bpp = global_multiboot_info->fb_bpp,
        .fb_addr = global_multiboot_info->fb_addr
    };
    memcpy(buf, &info, size);
}

void framebuffer_file_prepare(struct devfs_file* dfile, file_t* file) {
    file->size = sizeof(fb_info_t);
}

char* framebuffer_file_name(devfs_file_t* file) {
	return "framebuffer";
}

devfs_file_t framebuffer_file = {
	.read = framebuffer_file_read,
	.prepare = framebuffer_file_prepare,
	.name = framebuffer_file_name
};