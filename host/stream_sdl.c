#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charsize;
} psf1_header_t;

typedef struct {
    psf1_header_t* header;
    void* glyph_buffer;
} psf1_font_t;

const int SCREEN_WIDTH = 8 * 80;
const int SCREEN_HEIGHT = 16 * 25;

#define FONT "../cdrom/zap-light16.psf"

#define PORT 9999
#define FB_SIZE (80 * 25 * 2)

psf1_font_t psf1_buffer_to_font(void* buffer) {
    psf1_header_t* header = (psf1_header_t*)buffer;
    // assert(header->magic[0] == PSF1_MAGIC0 && header->magic[1] == PSF1_MAGIC1);

    void* glyph_buffer = (void*)(buffer + sizeof(psf1_header_t));

    psf1_font_t font = {.header = header, .glyph_buffer = glyph_buffer };

    return font;
}

static inline void set_pixel(SDL_Renderer* renderer, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawPoint(renderer, x, y);
}

void draw_char(SDL_Renderer* renderer, psf1_font_t* font, uint32_t x, uint32_t y, char c, uint32_t color, uint32_t bgcolor) {
    char* font_ptr = (char*)font->glyph_buffer + (c * font->header->charsize);

    for (unsigned long i = y; i < y + 16; i++) {
        for (unsigned long j = x; j < x + 8; j++) {
            if ((*font_ptr & (0b10000000 >> (j - x))) > 0) {
                set_pixel(renderer, j, i, (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, (color & 0x000000ff), (color & 0xff000000) >> 24);
            } else {
                set_pixel(renderer, j, i, (bgcolor & 0x00ff0000) >> 16, (bgcolor & 0x0000ff00) >> 8, (bgcolor & 0x000000ff), (bgcolor & 0xff000000) >> 24);
            }
        }
        font_ptr++;
    }
}

char buffer[FB_SIZE] = { 0 };
char buffer_old[FB_SIZE] = { 0 };

bool quit = false;

uint32_t color_translation_table[] = {
    0xFF000000, // black
    0xFF0000AA, // blue
    0xFF00AA00, // green
    0xFF00AAAA, // cyan
    0xFFAA0000, // red
    0xFFAA00AA, // magenta
    0xFFAA5500, // brown
    0xFFAAAAAA, // grey
    0xFF555555, // dark grey
    0xFF5555FF, // bright blue
    0xFF55FF55, // bright green
    0xFF00FFFF, // bright cyan
    0xFFFF5555, // bright red
    0xFFFF55FF, // bright magenta
    0xFFFFFF00, // yellow
    0xFFFFFFFF, // white
};

void* recv_thread(void* sock_ptr) {
    int sock = *(int*)sock_ptr;

    struct sockaddr_in client_addr = { 0 };
    socklen_t client_addr_len = sizeof(client_addr);

    while (!quit) {
        int recv_len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (recv_len != FB_SIZE) {
            printf("Invalid packet of size %d!\n", recv_len);
            continue;
        }
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    printf("MicroOS Stream server Copyright (C) 2023 Glowman554\n");

    printf("Loading font %s\n", FONT);
    FILE* f = fopen(FONT, "rb");
    if (!f) {
        printf("Failed to load font!\n");
        return -1;
    }
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    void* font_buf = malloc(fsize);
    fread(font_buf, fsize, 1, f);
    fclose(f);

    psf1_font_t font = psf1_buffer_to_font(font_buf);

    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket creation failed\n");
        return -1;
    }

    struct sockaddr_in server_addr = { 0 };
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("bind failed\n");
        close(sockfd);
        return -1;
    }

    printf("Ready for connections on port %d\n", PORT);

    pthread_t thread;
    int result = pthread_create(&thread, NULL, recv_thread, &sockfd);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("MicroOS Stream Server", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    if (!window) {
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    Uint32 frameStartTime, frameEndTime, frameDuration;

    while (!quit) {
        frameStartTime = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 80; j++) {
                int buffer_idx = 2 * (i * 80 + j);
                int buffer_idx_color = buffer_idx + 1;

                char chr = buffer[buffer_idx];
                char clr = buffer[buffer_idx_color];
                if (!chr || chr == ' ') {
                    continue;
                }
                
                uint32_t fg = color_translation_table[clr & 0x0f];
                uint32_t bg = color_translation_table[(clr & 0xf0) >> 4];

                draw_char(renderer, &font, j * 8, i * 16, chr, fg, bg);
            }
        }

        SDL_RenderPresent(renderer);

        frameEndTime = SDL_GetTicks();

        // Calculate the frame duration (time taken to render the frame)
        frameDuration = frameEndTime - frameStartTime;

        Uint32 targetFrameTime = 1000 / 30; // Convert target FPS to milliseconds
        if (frameDuration < targetFrameTime) {
            SDL_Delay(targetFrameTime - frameDuration);
        } else {
            printf("Frame took too long: %dms / %dms\n", frameDuration, targetFrameTime);
        }
    }

    free(font_buf);

    close(sockfd);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}