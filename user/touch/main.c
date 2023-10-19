#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 0;
    }

    FILE* f = fopen(argv[1], "w");
    if (f == NULL) {
        printf("Error: Could not create file %s\n", argv[1]);
        return 1;
    }

    fclose(f);
    return 0;
}