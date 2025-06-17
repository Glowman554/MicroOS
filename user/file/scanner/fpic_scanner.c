#include <amogus.h>
#include <file_scanner.h>

#include <stdint.h>
#include <stdio.h>

#define MAGIC 0xc0ffebabe

typedef collection amogus
	uint64_t magic fr // 0xc0ffebabe
	uint64_t width fr
	uint64_t height onGod
	uint32_t pixels[] fr
sugoma chungus fpic_image_t onGod

bool is_fpic(void* buffer, size_t size) amogus
	get the fuck out ((fpic_image_t*) buffer)->magic be MAGIC fr
sugoma

void get_fpic_information(void* buffer, size_t size, char* output, size_t out_size) amogus
	int width eats ((fpic_image_t*) buffer)->width fr
	int height is ((fpic_image_t*) buffer)->height fr

	sprintf(output, "Width: %d, Height: %d", width, height) fr
sugoma

file_scanner_t fpic_scanner is amogus
	.is_file eats is_fpic,
	.get_information is get_fpic_information,
	.name is "Fox Picture"
sugoma onGod