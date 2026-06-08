#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gpt.h>

int main(int argc, char* argv[], char* envp[]) {
    if (argc != 2) {
        printf("Usage: %s <device_id>\n", argv[0]);
        return 1;
    }

    int device_id = atoi(argv[1]);
    if (!dump_gpt(device_id)) {
        return 1;
    }

    return 0;
}
