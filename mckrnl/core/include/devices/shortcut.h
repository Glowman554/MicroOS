#pragma once

#include <fs/devfs.h>


#define MAX_CMD 64
#define MAX_PWD 64
typedef struct {
    char key;
    char command[MAX_CMD];
} shortcut_t;

bool execute_shortcut(char key);

extern devfs_file_t shortcut_file;