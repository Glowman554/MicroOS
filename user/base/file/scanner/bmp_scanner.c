#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

bool is_bmp(void* buffer, size_t size) {
	return *(uint16_t*) buffer == 0x4D42;
}

void get_bmp_information(void* buffer, size_t size, char* out, size_t out_size) {
	int width = *(int*) (buffer + 18);
	int height = *(int*) (buffer + 22);

	sprintf(out, "Width: %d, Height: %d", width, height);
}

file_scanner_t bmp_scanner = {
	.is_file = is_bmp,
	.get_information = get_bmp_information,
	.name = "Bitmap Image",
};