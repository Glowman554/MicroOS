#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

bool is_gz(void* buffer, size_t size) {
	return *(uint16_t*) buffer == 0x8b1f;
}

void get_gz_information(void* buffer, size_t size, char* out, size_t out_size) {
	uint8_t method = ((uint8_t*) buffer)[3];
	uint32_t decompressed_size = *((uint32_t*) &((uint8_t*) buffer)[size - 4]);

	sprintf(out, "method: %d, decompressed size: %d", method, decompressed_size);
}

file_scanner_t gz_scanner = {
	.is_file = is_gz,
	.get_information = get_gz_information,
	.name = "GZIP compressed data",
};