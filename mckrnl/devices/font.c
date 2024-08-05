#include <memory/vmm.h>
#include <devices/font.h>
#include <assert.h>
#include <string.h>

void font_file_read(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    assert(offset + size <= file->size);

    multiboot_module_t* mod = (multiboot_module_t*) dfile->driver_specific_data;
    memcpy(buf, (void*) (mod->mod_start + offset), size);
}

void font_file_prepare(struct devfs_file* dfile, file_t* file) {
    multiboot_module_t* mod = (multiboot_module_t*) dfile->driver_specific_data;
    file->size = mod->mod_end - mod->mod_start;
}

char* font_file_name(struct devfs_file* dfile) {
    return "font";
}

devfs_file_t* get_font_file(multiboot_module_t* font) {
    devfs_file_t* file = vmm_alloc(TO_PAGES(sizeof(devfs_file_t)));

    file->read = font_file_read;
    file->prepare = font_file_prepare;
    file->name = font_file_name;
    file->driver_specific_data = font;

    return file;
}
