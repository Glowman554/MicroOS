#include <non-standart/stdio.h>
#include <stdlib.h>
#include <string.h>

#include <non-standart/sys/file.h>
#include <non-standart/sys/env.h>

void print_tree(char* path, const char* prefix) {
    char* resolved = malloc(256);
    memset(resolved, 0, 256);
    if (!resolve_check(path, resolved, true)) {
        printf("%sError: No such file or directory: %s\n", prefix, path);
        free(resolved);
        return;
    }

    char* name = resolved;
    for (char* p = resolved; *p; p++) {
        if (*p == '/') {
            name = p + 1;
        }
    }

    int fd = open(resolved, FILE_OPEN_MODE_READ);
    if (fd != -1) {
        printf("%s%s\n", prefix, name);
        close(fd);
        free(resolved);
        return;
    }

 
    dir_t dir = {0};
    int idx = 0;
    dir_at(resolved, idx, &dir);

    while (!dir.is_none){
        printf("%s|- %s%s\n", prefix, dir.name, dir.type == ENTRY_DIR ? "/" : "");
        
        if (dir.type == ENTRY_DIR) {
            char* subpath = malloc(512);
            sprintf(subpath, "%s/%s", resolved, dir.name);

            char* new_prefix = malloc(256);
            sprintf(new_prefix, "%s%s", prefix, "|   ");

            print_tree(subpath, new_prefix);

            free(subpath);
            free(new_prefix);
        }

        idx = dir.idx + 1;
        dir_at(resolved, idx, &dir);
    }

    printf("%s\n", prefix);

    free(resolved);
}

int main(int argc, char *argv[]) {
	char start_path[256] = {0};
	set_env(SYS_GET_PWD_ID, start_path);

    if (argc > 1) {
		bool canresolve = resolve_check(argv[1], start_path, true);
		if (!canresolve) {
			printf("Error: No such file or directory: %s\n", argv[1]);
			return 1;
		}
    }
    
    printf("%s/\n", start_path);
    print_tree(start_path, "");
    return 0;
}