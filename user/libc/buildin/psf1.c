#include <buildin/psf1.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

psf1_font_t psf1_from_buffer(void* buffer) {
    psf1_header_t* header = (psf1_header_t*) buffer;
   	assert(header->magic[0] == PSF1_MAGIC0 && header->magic[1] == PSF1_MAGIC1);

	void* glyph_buffer = (void*) (buffer + sizeof(psf1_header_t));

	psf1_font_t font = {
		.header = header,
		.glyph_buffer = glyph_buffer
	};
	
	return font;
}


psf1_font_t psf1_from_file(const char* path) {
    FILE* fontf = fopen(path, "rb");
    assert(fontf);

    fsize(fontf, size);
    void* buffer = malloc(size);
    fread(buffer, size, 1, fontf);
    fclose(fontf);

    return psf1_from_buffer(buffer);
}