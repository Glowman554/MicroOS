#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

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
#define INPUT_BUFFER_SIZE 512

void die(const char* message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void print_usage(const char* program_name) {
    printf("Usage: %s <input.mkm> [output.mkm]\n", program_name);
    printf("Edit a MicroOS keymap file interactively. If output is omitted, the input file is updated in place.\n");
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

void trim_newline(char* text) {
    size_t len = strlen(text);
    if (len == 0) return;
    if (text[len - 1] == '\n') {
        text[len - 1] = '\0';
    }
    if (len > 1 && text[len - 2] == '\r') {
        text[len - 2] = '\0';
    }
}

bool string_equals_ignore_case(const char* a, const char* b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
        a++;
        b++;
    }
    return *a == *b;
}

uint32_t parse_hex_value(const char* text, bool* ok) {
    char* end = NULL;
    unsigned long value = strtoul(text, &end, 0);
    *ok = (end != text && *end == '\0' && value <= 0xFFFFFFFFUL);
    return (uint32_t)value;
}

keymap_t* load_keymap(const char* path, uint32_t* out_num_keymaps) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }

    long size = get_file_size(file);
    if (size < 0) {
        fclose(file);
        return NULL;
    }

    if (size < (long)sizeof(keymap_file_header_t)) {
        fclose(file);
        return NULL;
    }

    keymap_file_header_t header;
    if (fread(&header, sizeof(header), 1, file) != 1) {
        fclose(file);
        return NULL;
    }

    if (header.magic != KEYMAP_MAGIC || header.num_keymaps == 0) {
        fclose(file);
        return NULL;
    }

    long new_size = sizeof(keymap_file_header_t) + (long)header.num_keymaps * sizeof(keymap_t);
    long old_size = sizeof(keymap_file_header_t) + (long)header.num_keymaps * sizeof(keymap_old_t);

    keymap_t* maps = malloc((size_t)header.num_keymaps * sizeof(keymap_t));
    if (maps == NULL) {
        fclose(file);
        return NULL;
    }

    if (size == new_size) {
        if (fread(maps, sizeof(keymap_t), header.num_keymaps, file) != header.num_keymaps) {
            free(maps);
            fclose(file);
            return NULL;
        }
    } else if (size == old_size) {
        keymap_old_t* old_maps = malloc((size_t)header.num_keymaps * sizeof(keymap_old_t));
        if (old_maps == NULL) {
            free(maps);
            fclose(file);
            return NULL;
        }

        if (fread(old_maps, sizeof(keymap_old_t), header.num_keymaps, file) != header.num_keymaps) {
            free(old_maps);
            free(maps);
            fclose(file);
            return NULL;
        }

        for (uint32_t i = 0; i < header.num_keymaps; i++) {
            memcpy(maps[i].name, old_maps[i].name, sizeof(maps[i].name));
            memcpy(maps[i].layout_normal, old_maps[i].layout_normal, sizeof(maps[i].layout_normal));
            memcpy(maps[i].layout_shift, old_maps[i].layout_shift, sizeof(maps[i].layout_shift));
            memcpy(maps[i].layout_alt, old_maps[i].layout_alt, sizeof(maps[i].layout_alt));
            memset(maps[i].layout_altgr, 0, sizeof(maps[i].layout_altgr));
        }
        free(old_maps);
    } else {
        free(maps);
        fclose(file);
        return NULL;
    }

    fclose(file);
    *out_num_keymaps = header.num_keymaps;
    return maps;
}

bool save_keymap(const char* path, keymap_t* maps, uint32_t num_keymaps) {
    FILE* file = fopen(path, "wb");
    if (file == NULL) {
        return false;
    }

    keymap_file_header_t header = { .magic = KEYMAP_MAGIC, .num_keymaps = num_keymaps };
    if (fwrite(&header, sizeof(header), 1, file) != 1) {
        fclose(file);
        return false;
    }

    if (fwrite(maps, sizeof(keymap_t), num_keymaps, file) != num_keymaps) {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

void print_layouts(keymap_t* maps, uint32_t num_keymaps) {
    printf("Loaded %u keymap(s):\n", num_keymaps);
    for (uint32_t i = 0; i < num_keymaps; i++) {
        printf("  %2u: %s\n", i, maps[i].name[0] ? maps[i].name : "<unnamed>");
    }
}

int select_layout(keymap_t* maps, uint32_t num_keymaps) {
    char line[INPUT_BUFFER_SIZE];

    while (true) {
        print_layouts(maps, num_keymaps);
        printf("Choose layout by number or name: ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            die("Input error while selecting layout.");
        }
        trim_newline(line);
        if (line[0] == '\0') {
            continue;
        }

        bool ok;
        uint32_t index = parse_hex_value(line, &ok);
        if (ok && index < num_keymaps) {
            return (int)index;
        }

        for (uint32_t i = 0; i < num_keymaps; i++) {
            if (string_equals_ignore_case(line, maps[i].name)) {
                return (int)i;
            }
        }

        printf("Invalid layout '%s'. Enter a number or a known layout name.\n", line);
    }
}

int select_type(void) {
    const char* labels[] = {"normal", "shift", "alt", "altgr"};
    char line[INPUT_BUFFER_SIZE];

    while (true) {
        printf("Choose keymap type (normal, shift, alt, altgr): ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            die("Input error while selecting type.");
        }
        trim_newline(line);
        if (line[0] == '\0') {
            continue;
        }

        for (int i = 0; i < 4; i++) {
            if (string_equals_ignore_case(line, labels[i])) {
                return i;
            }
        }

        bool ok;
        uint32_t index = parse_hex_value(line, &ok);
        if (ok && index < 4) {
            return (int)index;
        }

        printf("Invalid type '%s'. Type must be one of: normal, shift, alt, altgr.\n", line);
    }
}

unsigned char* select_type_array(keymap_t* map, int type_index) {
    switch (type_index) {
        case 0: return (unsigned char*)map->layout_normal;
        case 1: return (unsigned char*)map->layout_shift;
        case 2: return (unsigned char*)map->layout_alt;
        case 3: return (unsigned char*)map->layout_altgr;
        default: return NULL;
    }
}

void print_byte_value(unsigned char value) {
    if (isprint(value)) {
        printf("0x%02x ('%c')", value, value);
    } else {
        printf("0x%02x", value);
    }
}

bool parse_edit_pair(const char* token, uint32_t* out_index, unsigned char* out_value) {
    const char* sep = strchr(token, '=');
    if (sep == NULL) {
        return false;
    }

    char index_text[64];
    char value_text[64];
    size_t left_len = sep - token;
    if (left_len >= sizeof(index_text)) {
        return false;
    }

    memcpy(index_text, token, left_len);
    index_text[left_len] = '\0';
    strncpy(value_text, sep + 1, sizeof(value_text) - 1);
    value_text[sizeof(value_text) - 1] = '\0';

    bool ok;
    uint32_t index = parse_hex_value(index_text, &ok);
    if (!ok || index > 0xff) {
        return false;
    }

    uint32_t value = parse_hex_value(value_text, &ok);
    if (!ok || value > 0xff) {
        return false;
    }

    *out_index = index;
    *out_value = (unsigned char)value;
    return true;
}

void perform_edit_pass(keymap_t* map, int type_index) {
    char line[INPUT_BUFFER_SIZE];

    while (true) {
        printf("Enter edits as location=char pairs in hex, separated by spaces or commas (for example 0x1c=0x61 0x1d=0x62).\n");
        printf("Type DONE to finish, or CHANGE to switch layout/type.\n> ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            die("Input error while editing.");
        }
        trim_newline(line);

        if (line[0] == '\0') {
            continue;
        }

        if (string_equals_ignore_case(line, "DONE")) {
            break;
        }
        if (string_equals_ignore_case(line, "CHANGE")) {
            break;
        }

        unsigned char* array = select_type_array(map, type_index);
        if (array == NULL) {
            die("Invalid keymap type selected.");
        }

        bool any = false;
        char* token = strtok(line, " ,\t");
        while (token != NULL) {
            uint32_t index;
            unsigned char value;
            if (!parse_edit_pair(token, &index, &value)) {
                printf("Invalid edit token '%s'. Use format location=char, both in hex.\n", token);
            } else {
                unsigned char previous = array[index];
                array[index] = value;
                printf("Updated 0x%02x: ", index);
                print_byte_value(previous);
                printf(" -> ");
                print_byte_value(value);
                printf("\n");
                any = true;
            }
            token = strtok(NULL, " ,\t");
        }

        if (!any) {
            printf("No valid edits were applied.\n");
        }
    }
}

void run_editor(keymap_t* maps, uint32_t num_keymaps) {
    while (true) {
        int layout_index = select_layout(maps, num_keymaps);
        int type_index = select_type();
        printf("Editing layout '%s' type '%s'.\n",
               maps[layout_index].name[0] ? maps[layout_index].name : "<unnamed>",
               type_index == 0 ? "normal" : type_index == 1 ? "shift" : type_index == 2 ? "alt" : "altgr");
        perform_edit_pass(&maps[layout_index], type_index);

        char line[INPUT_BUFFER_SIZE];
        while (true) {
            printf("Would you like to edit another layout/type? (y/n): ");
            if (fgets(line, sizeof(line), stdin) == NULL) {
                die("Input error while confirming edits.");
            }
            trim_newline(line);
            if (line[0] == '\0') {
                continue;
            }
            if (string_equals_ignore_case(line, "y") || string_equals_ignore_case(line, "yes")) {
                break;
            }
            if (string_equals_ignore_case(line, "n") || string_equals_ignore_case(line, "no")) {
                return;
            }
            printf("Enter 'y' or 'n'.\n");
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_path = argv[1];
    const char* output_path = (argc == 3 ? argv[2] : argv[1]);
    bool in_place = (argc == 2);

    uint32_t num_keymaps = 0;
    keymap_t* maps = load_keymap(input_path, &num_keymaps);
    if (maps == NULL) {
        fprintf(stderr, "Failed to load keymap file '%s'. Ensure it is a valid keymap file.\n", input_path);
        return EXIT_FAILURE;
    }

    run_editor(maps, num_keymaps);

    char temp_path[512];
    const char* write_path = output_path;
    if (in_place) {
        snprintf(temp_path, sizeof(temp_path), "%s.tmp", input_path);
        write_path = temp_path;
    }

    if (!save_keymap(write_path, maps, num_keymaps)) {
        free(maps);
        fprintf(stderr, "Failed to write updated keymap file '%s'.\n", write_path);
        return EXIT_FAILURE;
    }

    if (in_place) {
        if (rename(temp_path, input_path) != 0) {
            perror("Failed to replace input file with edited file");
            free(maps);
            return EXIT_FAILURE;
        }
    }

    free(maps);
    printf("Saved updated keymap to %s\n", output_path);
    return EXIT_SUCCESS;
}
