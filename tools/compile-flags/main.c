#include <stdio.h>

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (i + 1 < argc && argv[i + 1][0] != '-') {
            printf("%s%s\n", argv[i], argv[i + 1]);
            i++;
        } else {
            printf("%s\n", argv[i]);
        }
    }
    return 0;
}