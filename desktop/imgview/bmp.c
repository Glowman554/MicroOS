#include <scanner.h>
#include <stdint.h>


bool test_format_bmp(void* file_buffer, int file_size) {
	uint8_t* bmp_data = (uint8_t*) file_buffer;
	return bmp_data[0] == 'B' && bmp_data[1] == 'M';
}

uint32_t get_pixel_bmp(void* file_buffer, int file_size, int x, int y) {
	uint8_t* bmp_data = (uint8_t*) file_buffer;
	
	int width, height;
	bmp_scanner.get_size(file_buffer, file_size, &width, &height);

    int bpp = *(int16_t*) (bmp_data + 28);
    int row_size = width * bpp / 8;
    int padding = (4 - (row_size % 4)) % 4;
    int offset = 54 + (height - y - 1) * (row_size + padding);
    uint8_t* row = bmp_data + offset;
    uint8_t* pixel = row + x * bpp / 8;
    uint32_t argb = 0;
    if (bpp == 32) {
    	argb = *(uint32_t*) pixel;
	} else if (bpp == 24) {
    	argb = 0xff000000 | *(uint32_t*) pixel;
    } else if (bpp == 8) {
    	argb = 0xff000000 | *pixel << 16 | *pixel << 8 | *pixel;
    }
	return argb;
}

void get_size_bmp(void* file_buffer, int file_size, int* width, int* height) {
	uint8_t* bmp_data = (uint8_t*) file_buffer;
	*width = *(int32_t*) (bmp_data + 18);
	*height = *(int32_t*) (bmp_data + 22);
}

format_scanner_t bmp_scanner = {
	.test_format = test_format_bmp,
	.get_pixel = get_pixel_bmp,
	.get_size = get_size_bmp
};