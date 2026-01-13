#include <window.h>
#include <window/font.h>
#include <window/fpic.h>
#include <window/strinput.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <scanner.h>

#define MIN_SIZE_PX 200

void draw_image_format_scaled(void* file_buffer, int file_size, format_scanner_t* format, int x, int y, int scale) {
    int width, height;
    format->get_size(file_buffer, file_size, &width, &height);

    for (int i = 0; i < width * scale; i++) {
        for (int j = 0; j < height * scale; j++) {
            set_pixel_window(x + i, y + j, format->get_pixel(file_buffer, file_size, i / scale, j / scale));
        }
    }
}

int main(int argc, char* argv[]) {
    window_init(200, 400, 50, 50, "Image Viewer");
    window_fullscreen();

    char inputBuffer[64] = { 0 };
    const char* file;
	if (argc != 2) {
        psf1_font_t font = load_psf1_font("dev:font");
        
        strinput_t input = {
            .x = 7 * 8,
            .y = 0,
            .bgcolor = 0,
            .fgcolor = 0xffffffff,
            .buffer = inputBuffer,
            .idx = 0
        };


		while (strinput(&font, &input)) {
			window_optimize();

			if (window_redrawn()) {
				window_clear(0);
				draw_string_window(&font, 0, 0, "path > ", 0xffffffff, 0);
			}
		}

        file = inputBuffer;
	} else {
        file = argv[1];
    }

    // printf("Loading %s...\n", file);
    FILE* f = fopen(file, "rb");
    if (!f) {
        // printf("Failed to load fpic %s!\n", file);
        return -1;
    }

    fsize(f, size);

    void* buffer = malloc(size);
    fread(buffer, size, 1, f);
    fclose(f);

    format_scanner_t* format = get_scanner(buffer, size);

    int width, height;
    format->get_size(buffer, size, &width, &height);

    int scale = 1;
    if (width < MIN_SIZE_PX || height < MIN_SIZE_PX) {
        scale = MAX(MIN_SIZE_PX / width, MIN_SIZE_PX / height);
    }

    // printf("Using scale factor %d\n", scale);

    window->window_width = width * scale;
    window->window_height = height * scale;
    // window->dirty = true;

    while (true) {
        window_optimize();
        if (window_redrawn()) {
            scale = MIN(window->window_height / height, window->window_width / width);
            draw_image_format_scaled(buffer, size, format, 0, 0, scale);
        }
    }
}