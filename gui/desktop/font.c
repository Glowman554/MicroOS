#include <font.h>
#include <types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/file.h>

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

psf1_font_t psf1_buffer_to_font(void* buffer) {
    psf1_header_t* header = (psf1_header_t*)buffer;
    void* glyph_buffer = (void*)((uint8_t*)buffer + sizeof(psf1_header_t));
    psf1_font_t font = {.header = header, .glyph_buffer = glyph_buffer };
    return font;
}

psf1_font_t load_psf1_font(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        psf1_font_t ret = {};
        return ret;
    }
    
    fsize(f, size);
    uint8_t* buffer = malloc(size);
    fread(buffer, size, 1, f);
    fclose(f);
    
    return psf1_buffer_to_font(buffer);
}
