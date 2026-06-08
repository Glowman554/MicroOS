#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct keymap_file_header_t {
    uint32_t magic;
    uint32_t num_keymaps; // 0xab1589fd
} keymap_file_header_t;

typedef struct keymap_old_t {
    char name[16];
    char layout_normal[0xff];
    char layout_shift[0xff];
    char layout_alt[0xff];
} keymap_old_t;

typedef struct keymap_t {
    char name[16];
    char layout_normal[0xff];
    char layout_shift[0xff];
    char layout_alt[0xff];
    char layout_altgr[0xff];
} keymap_t;

#define KEYMAP_MAGIC 0xab1589fd

void print_usage(const char* program_name) {
    printf("Usage: %s <input.old.mkm> <output.mkm>\n", program_name);
    printf("Converts legacy keymap files to the new altgr-aware keymap format.\n");
}

void die(const char* message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

long get_file_size(FILE* file) {
    if (fseek(file, 0, SEEK_END) != 0) {
        return -1;
    }
    long size = ftell(file);
    if (size < 0) {
        return -1;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        return -1;
    }
    return size;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_path = argv[1];
    const char* output_path = argv[2];

    FILE* input = fopen(input_path, "rb");
    if (input == NULL) {
        perror("Failed to open input file");
        return EXIT_FAILURE;
    }

    long input_size = get_file_size(input);
    if (input_size < 0) {
        fclose(input);
        die("Failed to determine input file size.");
    }

    if (input_size < (long)sizeof(keymap_file_header_t)) {
        fclose(input);
        die("Input file is too small to contain a valid keymap header.");
    }

    keymap_file_header_t header;
    if (fread(&header, sizeof(header), 1, input) != 1) {
        fclose(input);
        die("Failed to read keymap header from input file.");
    }

    if (header.magic != KEYMAP_MAGIC) {
        fclose(input);
        die("Input file has an invalid keymap magic number.");
    }

    if (header.num_keymaps == 0) {
        fclose(input);
        die("Input keymap file contains zero keymaps.");
    }

    long expected_old_size = sizeof(keymap_file_header_t) + (long)header.num_keymaps * sizeof(keymap_old_t);
    if (input_size != expected_old_size) {
        fclose(input);
        fprintf(stderr, "Input file size mismatch: expected %ld bytes, got %ld bytes.\n", expected_old_size, input_size);
        return EXIT_FAILURE;
    }

    keymap_old_t* old_maps = malloc((size_t)header.num_keymaps * sizeof(keymap_old_t));
    if (old_maps == NULL) {
        fclose(input);
        die("Out of memory allocating old keymap buffer.");
    }

    if (fread(old_maps, sizeof(keymap_old_t), header.num_keymaps, input) != header.num_keymaps) {
        free(old_maps);
        fclose(input);
        die("Failed to read legacy keymaps from input file.");
    }
    fclose(input);

    long output_size = sizeof(keymap_file_header_t) + (long)header.num_keymaps * sizeof(keymap_t);
    char* output_buffer = malloc((size_t)output_size);
    if (output_buffer == NULL) {
        free(old_maps);
        die("Out of memory allocating output keymap buffer.");
    }

    keymap_file_header_t* out_header = (keymap_file_header_t*)output_buffer;
    out_header->magic = KEYMAP_MAGIC;
    out_header->num_keymaps = header.num_keymaps;

    keymap_t* out_maps = (keymap_t*)(output_buffer + sizeof(keymap_file_header_t));
    for (uint32_t i = 0; i < header.num_keymaps; i++) {
        keymap_old_t* old = &old_maps[i];
        keymap_t* out = &out_maps[i];

        memcpy(out->name, old->name, sizeof(out->name));
        memcpy(out->layout_normal, old->layout_normal, sizeof(out->layout_normal));
        memcpy(out->layout_shift, old->layout_shift, sizeof(out->layout_shift));
        memcpy(out->layout_alt, old->layout_alt, sizeof(out->layout_alt));
        memset(out->layout_altgr, 0, sizeof(out->layout_altgr));
    }

    free(old_maps);

    FILE* output = fopen(output_path, "wb");
    if (output == NULL) {
        free(output_buffer);
        perror("Failed to open output file");
        return EXIT_FAILURE;
    }

    if (fwrite(output_buffer, output_size, 1, output) != 1) {
        free(output_buffer);
        fclose(output);
        die("Failed to write updated keymap file.");
    }

    fclose(output);
    free(output_buffer);

    printf("Converted %u keymaps from %s to %s\n", header.num_keymaps, input_path, output_path);
    return EXIT_SUCCESS;
}
