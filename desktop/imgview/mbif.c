#include <scanner.h>

typedef struct {
    int32_t width, height, xOff, yOff;
    int64_t size;
	uint32_t imageBuffer[];
} mbif_image_t;


bool test_format_mbif(void* file_buffer, int file_size) {
	mbif_image_t* image = (mbif_image_t*) file_buffer;
	return image->size == (image->width * image->height * 4);
}

uint32_t get_pixel_mbif(void* file_buffer, int file_size, int x, int y) {
	mbif_image_t* image = (mbif_image_t*) file_buffer;
    return image->imageBuffer[y * image->width + x];
}

void get_size_mbif(void* file_buffer, int file_size, int* width, int* height) {
	mbif_image_t* image = (mbif_image_t*) file_buffer;
	*width = image->width;
	*height = image->height;
}

format_scanner_t mbif_scanner = {
	.test_format = test_format_mbif,
	.get_pixel = get_pixel_mbif,
	.get_size = get_size_mbif
};