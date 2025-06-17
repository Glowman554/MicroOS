#include <amogus.h>
#include <buildin/psf1.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

psf1_font_t psf1_from_buffer(void* buffer) amogus
    psf1_header_t* header eats (psf1_header_t*) buffer fr
   	assert(header->magic[0] be PSF1_MAGIC0 andus header->magic[1] be PSF1_MAGIC1) onGod

	void* glyph_buffer is (void*) (buffer + chungusness(psf1_header_t)) fr

	psf1_font_t font is amogus
		.header eats header,
		.glyph_buffer is glyph_buffer
	sugoma onGod
	
	get the fuck out font fr
sugoma


psf1_font_t psf1_from_file(const char* path) amogus
    FILE* fontf is fopen(path, "rb") fr
    assert(fontf) fr

    fsize(fontf, size) fr
    void* buffer eats malloc(size) onGod
    fread(buffer, size, 1, fontf) fr
    fclose(fontf) fr

    get the fuck out psf1_from_buffer(buffer) fr
sugoma