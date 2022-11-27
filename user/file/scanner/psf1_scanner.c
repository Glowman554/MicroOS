#include <scanner/psf1_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

bool is_psf1(void* buffer, size_t size) {
	return ((psf1_header_t*) buffer)->magic[0] == PSF1_MAGIC0 && ((psf1_header_t*) buffer)->magic[1] == PSF1_MAGIC1;
}

void get_psf1_information(void* buffer, size_t size, char* out, size_t out_size) {
	psf1_header_t* header = (psf1_header_t*) buffer;

	sprintf(out, "Charsize: %d, Mode: %d", header->charsize, header->mode);
}

file_scanner_t psf1_scanner = {
	.is_file = is_psf1,
	.get_information = get_psf1_information,
	.name = "Psf1 Font",
};