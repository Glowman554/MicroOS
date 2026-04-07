#include <scanner/fm_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

bool is_figl(void* buffer, size_t size) {
	uint32_t magic = *(uint32_t*) buffer;
	return magic == 0xf181babe;
}

void get_figl_information(void* buffer, size_t size, char* out, size_t out_size) {
	sprintf(out, "Data section is %d bytes big", size - sizeof(uint32_t) - 255 * sizeof(uint32_t));
}

file_scanner_t figl_scanner = {
	.is_file = is_figl,
	.get_information = get_figl_information,
	.name = "Figlet font"
};