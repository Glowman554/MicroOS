#include <scanner.h>

typedef struct {
	uint64_t magic; // 0xc0ffebabe
	uint64_t width;
	uint64_t height;
	uint32_t pixels[];
} __attribute__((packed)) fpic_image_t;



bool test_format_fpic(void* file_buffer, int file_size) {
	fpic_image_t* image = (fpic_image_t*) file_buffer;
	return image->magic == 0xc0ffebabe;
}

uint32_t get_pixel_fpic(void* file_buffer, int file_size, int x, int y) {
	fpic_image_t* image = (fpic_image_t*) file_buffer;
    return image->pixels[y * image->width + x];
}

void get_size_fpic(void* file_buffer, int file_size, int* width, int* height) {
	fpic_image_t* image = (fpic_image_t*) file_buffer;
	*width = image->width;
	*height = image->height;
}

format_scanner_t fpic_scanner = {
	.test_format = test_format_fpic,
	.get_pixel = get_pixel_fpic,
	.get_size = get_size_fpic
};