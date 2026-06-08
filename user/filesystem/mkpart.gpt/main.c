#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <gpt.h>

int main(int argc, char* argv[], char* envp[]) {
    if (argc < 3) {
        printf("Usage: %s <device_id> --size <sectors> [--type <type>] [--name <name>]\n", argv[0]);
        return 1;
    }

    printf("mkpart.gpt: adding partition to disk %s\n", argv[1]);

    int device_id = atoi(argv[1]);
    uint64_t size = 0;
    char type_text[64] = "basic-data";
    char name[64] = {0};
    bool got_size = false;

    for (int idx = 2; idx < argc; idx++) {
        if (strcmp(argv[idx], "--size") == 0 && idx + 1 < argc) {
            bool ok;
            size = parse_size(argv[++idx], &ok);
            if (!ok || size == 0) {
                printf("mkpart.gpt: invalid size '%s'\n", argv[idx]);
                return 1;
            }
            got_size = true;
        } else if (strcmp(argv[idx], "--type") == 0 && idx + 1 < argc) {
            strncpy(type_text, argv[++idx], sizeof(type_text) - 1);
        } else if (strcmp(argv[idx], "--name") == 0 && idx + 1 < argc) {
            strncpy(name, argv[++idx], sizeof(name) - 1);
        } else {
            printf("mkpart.gpt: unknown argument '%s'\n", argv[idx]);
            return 1;
        }
    }

    if (!got_size) {
        printf("mkpart.gpt: --size is required\n");
        return 1;
    }

    uint8_t type_guid[16];
    if (!parse_type_guid(type_text, type_guid)) {
        printf("mkpart.gpt: invalid partition type '%s'\n", type_text);
        return 1;
    }

    if (!add_gpt_partition(device_id, size, type_guid, name[0] ? name : NULL)) {
        return 1;
    }

    return 0;
}
