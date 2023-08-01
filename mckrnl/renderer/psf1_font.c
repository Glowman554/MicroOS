#include <renderer/psf1_font.h>

#include <assert.h>

psf1_font_t psf1_buffer_to_font(void* buffer) {
    psf1_header_t* header = (psf1_header_t*) buffer;
   	assert(header->magic[0] == PSF1_MAGIC0 && header->magic[1] == PSF1_MAGIC1);

	void* glyph_buffer = (void*) (buffer + sizeof(psf1_header_t));

	psf1_font_t font = {
		.header = header,
		.glyph_buffer = glyph_buffer
	};
	
	return font;
}