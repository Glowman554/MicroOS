#pragma once

#include <fs/vfs.h>

typedef struct hostfs {
    vfs_mount_t mount;
    char mount_name[64];
    char host_root[512];
} hostfs_t;

hostfs_t *hostfs_create(const char *mount_name, const char *host_dir);
