#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void ask_int(FILE* cfg, const char* key, const char* description) {
    printf("%s (int)? ", description);
    
    char buf[32] = { 0 };
    gets(buf);

    int value = atoi(buf);
    printf("\nSetting '%s' to %d\n", key, value);

    fprintf(cfg, "%s=%d\n", key, value);
}

int main(int argc, char* argv[]) {
    char cfgFile[128] = { 0 };
    sprintf(cfgFile, "%sopt/desktop/desktop.cfg", getenv("ROOT_FS"));

    FILE* cfg = fopen(cfgFile, "w");
    assert(cfg);

    if (argc == 4) {
        fseek(cfg, 0, SEEK_END);

        if (strcmp(argv[1], "program") == 0) {
            printf("Adding program %s to config\n", argv[2]);
            fprintf(cfg, "\nprogram_%s=%s", argv[2], argv[3]);
        } else if (strcmp(argv[1], "extension") == 0) {
            printf("Adding file extension %s for program %s to config\n", argv[2], argv[3]);
            fprintf(cfg, "\nextension_%s=%s", argv[2], argv[3]);
        } else if (strcmp(argv[1], "shortcut") == 0) {
            printf("Adding shortcut for file %s to config\n", argv[2], argv[3]);
            fprintf(cfg, "\nshortcut_%s=%s", argv[2], argv[3]);
        } else {
            abort();
        }
    } else if (argc == 1) {
        ask_int(cfg, "desktop_scale", "Desktop scale");
    } else {
        abort();
    }

    fclose(cfg);
    printf("Desktop config saved in %s!\n", cfgFile);
    return 0;
}