#include <file_scanner.h>

#include <stdint.h>
#include <stdio.h>

#define MAGIC 0xc0ffebabe

typedef struct {
	uint64_t magic; // 0xc0ffebabe
	uint64_t width;
	uint64_t height;
	uint32_t pixels[];
} __attribute__((packed)) fpic_image_t;

bool is_fpic(void* buffer, size_t size) {
	return ((fpic_image_t*) buffer)->magic == MAGIC;
}

void get_fpic_information(void* buffer, size_t size, char* out, size_t out_size) {
	int width = ((fpic_image_t*) buffer)->width;
	int height = ((fpic_image_t*) buffer)->height;

	sprintf(out, "Width: %d, Height: %d", width, height);
}

file_scanner_t fpic_scanner = {
	.is_file = is_fpic,
	.get_information = get_fpic_information,
	.name = "Fox Picture"
};