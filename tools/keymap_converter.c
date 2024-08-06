#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct keymap_t {
    char name[16];
	char layout_normal[0xff];
	char layout_shift[0xff];
	char layout_alt[0xff];
} keymap_t;

typedef struct keymap_foxos_t {
	char layout_normal[0xff];
	char layout_shift[0xff];
	char layout_alt[0xff];
} keymap_foxos_t;


typedef struct keymap_file_header_t {
    uint32_t magic;
    uint32_t num_keymaps; // 0xab1589fd
} keymap_file_header_t;

keymap_foxos_t* load_keymap(char* name) {
    char path[128] = { 0 };
    sprintf(path, "../res/%s.fmp", name);

    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        printf("Failed to open %s\n", path);
        abort();
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    assert(size == sizeof(keymap_foxos_t));

    keymap_foxos_t* buffer = malloc(size);
    fread(buffer, size, 1, file);

    fclose(file);

    return buffer;
}

#define OUTPUT "keymap.mkm"

int main() {
	printf("MicroOS Keymap converter Copyright (C) 2023 Glowman554\n");

    char* keymaps[] = {
        "de",
        "us",
        "fr"
    };
    int num_keymaps = sizeof(keymaps) / sizeof(keymaps[0]);

    int file_size = sizeof(keymap_file_header_t) + sizeof(keymap_t) * num_keymaps;

    char* buffer = malloc(file_size);
    memset(buffer, 0, file_size);

    keymap_file_header_t* header = (keymap_file_header_t*) buffer;
    header->magic = 0xab1589fd;
    header->num_keymaps = num_keymaps;

    for (int i = 0; i < num_keymaps; i++) {
        printf("Including keymap for %s...\n", keymaps[i]);
        keymap_t* keymap = (keymap_t*) &buffer[sizeof(keymap_file_header_t) + sizeof(keymap_t) * i];
        strcpy(keymap->name, keymaps[i]);

        keymap_foxos_t* old = load_keymap(keymaps[i]);
        for (int j = 0; j < 0xff; j++) {
            keymap->layout_normal[j] = old->layout_normal[j];
            keymap->layout_shift[j] = old->layout_shift[j];
            keymap->layout_alt[j] = old->layout_alt[j];
        }

        free(old);
    }

    FILE* map = fopen(OUTPUT, "wb");
    if (map == NULL) {
        printf("Failed to open %s\n", OUTPUT);
        abort();
    }

    fwrite(buffer, file_size, 1, map);
    fclose(map);

    printf("Wrote %s!\n", OUTPUT);

    return 0;
}