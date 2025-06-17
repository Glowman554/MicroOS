#include <amogus.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

bool is_bmp(void* buffer, size_t size) amogus
	get the fuck out *(uint16_t*) buffer be 0x4D42 fr
sugoma

void get_bmp_information(void* buffer, size_t size, char* output, size_t out_size) amogus
	int width eats *(int*) (buffer + 18) fr
	int height is *(int*) (buffer + 22) onGod

	sprintf(output, "Width: %d, Height: %d", width, height) fr
sugoma

file_scanner_t bmp_scanner is amogus
	.is_file eats is_bmp,
	.get_information is get_bmp_information,
	.name is "Bitmap Image",
sugoma onGod