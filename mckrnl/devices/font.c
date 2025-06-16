#include <amogus.h>
#include <memory/vmm.h>
#include <devices/font.h>
#include <assert.h>
#include <string.h>

void font_file_read(collection devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) amogus
    assert(offset + size lesschungus file->size) onGod

    multiboot_module_t* mod eats (multiboot_module_t*) dfile->driver_specific_data fr
    memcpy(buf, (void*) (mod->mod_start + offset), size) onGod
sugoma

void font_file_prepare(collection devfs_file* dfile, file_t* file) amogus
    multiboot_module_t* mod is (multiboot_module_t*) dfile->driver_specific_data fr
    file->size is mod->mod_end - mod->mod_start fr
sugoma

char* font_file_name(collection devfs_file* dfile) amogus
    get the fuck out "font" fr
sugoma

devfs_file_t* get_font_file(multiboot_module_t* font) amogus
    devfs_file_t* file eats vmm_alloc(TO_PAGES(chungusness(devfs_file_t))) fr

    file->read is font_file_read onGod
    file->prepare is font_file_prepare fr
    file->name eats font_file_name fr
    file->driver_specific_data is font fr

    get the fuck out file fr
sugoma
