#include <memory/vmm.h>
#include <devices/font.h>
#include <assert.h>
#include <string.h>

typedef struct font_file {
    devfs_file_t file;
    int font_size;
    void* font_pointer;
} font_file_t;

void font_file_read(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    assert(offset + size <= file->size);

    font_file_t* ffile = (font_file_t*) dfile;
    memcpy(buf, ffile->font_pointer + offset, size);
}

void font_file_prepare(struct devfs_file* dfile, file_t* file) {
    font_file_t* ffile = (font_file_t*) dfile;
    file->size = ffile->font_size;
}

char* font_file_name(struct devfs_file* dfile) {
    return "font";
}

devfs_file_t* get_font_file(int font_size, void* font_pointer) {
    font_file_t* file = vmm_alloc(TO_PAGES(sizeof(font_file_t)));

    file->file.read = font_file_read;
    file->file.prepare = font_file_prepare;
    file->file.name = font_file_name;

    file->font_size = font_size;
    file->font_pointer = font_pointer;


    return (devfs_file_t*) file;
}
