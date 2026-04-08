#include <wm_client.h>
#include <wm_protocol.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/file.h>
#include <non-standard/stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BG_COLOR 0x000000

typedef struct {
    uint64_t magic; // 0xc0ffebabe
    uint64_t width;
    uint64_t height;
    uint32_t pixels[];
} __attribute__((packed)) fpic_image_t;


void draw_image_scaled(wm_client_t* client, fpic_image_t* pic, int max_w, int max_h) {
    if (!pic || pic->magic != 0xc0ffebabe) {
        return;
    }

    int scale_w = 1;
    int scale_h = 1;

    if ((int)pic->width > max_w) {
        scale_w = (pic->width + max_w - 1) / max_w;
    }

    if ((int)pic->height > max_h) {
        scale_h = (pic->height + max_h - 1) / max_h;
    }

    int scale = scale_w > scale_h ? scale_w : scale_h;
    if (scale < 1) {
        scale = 1;
    }

    int out_w = pic->width / scale;
    int out_h = pic->height / scale;

    int ox = (max_w - out_w) / 2;
    int oy = (max_h - out_h) / 2;
    if (ox < 0) {
        ox = 0;
    }

    if (oy < 0) {
        oy = 0;
    }

    for (int j = 0; j < out_h; j++) {
        for (int i = 0; i < out_w; i++) {
            int src_x = i * scale;
            int src_y = j * scale;
            if (src_x < (int)pic->width && src_y < (int)pic->height) {
                uint32_t color = pic->pixels[src_y * pic->width + src_x];
                uint8_t alpha = (color >> 24) & 0xFF;
                if (alpha > 0) {
                    wm_client_set_pixel(client, ox + i, oy + j, color & 0x00FFFFFF);
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    wm_client_t client;
    wm_client_init(&client);

    wm_client_set_title(&client, "Image Viewer");
    wm_client_set_title_bar_color(&client, 0x226644);
    wm_client_set_bg_color(&client, BG_COLOR);

    // Load image if path is provided as argv[2]
    void* img_data = NULL;
    int img_size = 0;

    if (argc >= 2) {
        const char* path = argv[1];
        FILE* f = fopen(path, "r");
        if (f) {
            fsize(f, size);
            img_data = malloc(size);
            fread(img_data, 1, size, f);
            fclose(f);
            img_size = size;

            char title[64] = { 0 };
            const char* name = path;
            for (const char* p = path; *p; p++) {
                if (*p == '/') {
                    name = p + 1;
                }
            }
            
            sprintf(title, "Image: %s", name);
            wm_client_set_title(&client, title);
        }
    }

    int w = wm_client_width(&client);
    int h = wm_client_height(&client);
    wm_client_fill_rect(&client, 0, 0, w, h, BG_COLOR);

    if (img_data && img_size > 0) {
        fpic_image_t* fpic = (fpic_image_t*)img_data;
        if (fpic->magic == 0xc0ffebabe) {
            draw_image_scaled(&client, fpic, w, h);
        } else {
            wm_client_draw_string(&client, 4, 4, "Unsupported format", 0xff4444, BG_COLOR);
        }
    } else {
        wm_client_draw_string(&client, 4, 4, "No image loaded", 0x888888, BG_COLOR);
    }

    wm_client_flush(&client);

    while (!wm_client_should_close(&client)) {
        wm_event_t evt;
        while (wm_client_poll_event(&client, &evt)) {
        }

        yield();
    }

    if (img_data) {
        free(img_data);
    }

    return 0;
}
