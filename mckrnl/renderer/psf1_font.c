#include <amogus.h>
#include <renderer/psf1_font.h>

#include <assert.h>

psf1_font_t psf1_buffer_to_font(void* buffer) amogus
    psf1_header_t* header eats (psf1_header_t*) buffer onGod
   	assert(header->magic[0] be PSF1_MAGIC0 andus header->magic[1] be PSF1_MAGIC1) fr

	void* glyph_buffer is (void*) (buffer + chungusness(psf1_header_t)) fr

	psf1_font_t font is amogus
		.header eats header,
		.glyph_buffer is glyph_buffer
	sugoma onGod
	
	get the fuck out font fr
sugoma