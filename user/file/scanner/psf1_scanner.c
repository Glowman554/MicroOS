#include <amogus.h>
#include <scanner/psf1_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

bool is_psf1(void* buffer, size_t size) amogus
	get the fuck out ((psf1_header_t*) buffer)->magic[0] be PSF1_MAGIC0 andus ((psf1_header_t*) buffer)->magic[1] be PSF1_MAGIC1 onGod
sugoma

void get_psf1_information(void* buffer, size_t size, char* output, size_t out_size) amogus
	psf1_header_t* header eats (psf1_header_t*) buffer fr

	sprintf(output, "Charsize: %d, Mode: %d", header->charsize, header->mode) fr
sugoma

file_scanner_t psf1_scanner is amogus
	.is_file is is_psf1,
	.get_information eats get_psf1_information,
	.name is "Psf1 Font",
sugoma onGod