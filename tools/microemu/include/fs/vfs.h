#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define VFS_MAX_MOUNTS 16
#define VFS_MAX_FDS    256
#define VFS_FD_OFFSET  5

enum file_open_mode {
    FILE_OPEN_MODE_READ,
    FILE_OPEN_MODE_WRITE,
    FILE_OPEN_MODE_READ_WRITE
};

enum dir_entry_type {
    ENTRY_FILE,
    ENTRY_DIR
};

typedef struct vfs_mount vfs_mount_t;

typedef struct file {
    vfs_mount_t *mount;
    int mode;
    void *driver_specific_data;
    char path[512];
    size_t size;
} file_t;

typedef struct dir_entry {
    char name[256];
    int idx;
    bool is_none;
    int type;
} dir_t;

struct vfs_mount {
    file_t *(*open)(vfs_mount_t *mount, char *path, int flags);
    void (*close)(vfs_mount_t *mount, file_t *file);
    void (*read)(vfs_mount_t *mount, file_t *file, void *buf, size_t size, size_t offset);
    void (*write)(vfs_mount_t *mount, file_t *file, void *buf, size_t size, size_t offset);
    void (*_delete)(vfs_mount_t *mount, file_t *file);
    void (*mkdir)(vfs_mount_t *mount, char *path);
    void (*touch)(vfs_mount_t *mount, char *path);
    dir_t (*dir_at)(vfs_mount_t *mount, int idx, char *path);
    void (*delete_dir)(vfs_mount_t *mount, char *path);
    void (*truncate)(vfs_mount_t *mount, file_t *file, size_t new_size);
    char *(*name)(vfs_mount_t *mount);
    void *driver_specific_data;
};

void vfs_init(void);
void vfs_mount(vfs_mount_t *mount);
vfs_mount_t *vfs_find_mount(char *path, char *file_path_out);

file_t *vfs_open(char *path, int flags);
void vfs_close(file_t *file);
void vfs_read(file_t *file, void *buf, size_t size, size_t offset);
void vfs_write(file_t *file, void *buf, size_t size, size_t offset);
void vfs_delete(file_t *file);
void vfs_truncate(file_t *file, size_t new_size);

// Directory operations
void vfs_mkdir(char *path);
void vfs_touch(char *path);
dir_t vfs_dir_at(int idx, char *path);
void vfs_delete_dir(char *path);
bool vfs_fs_at(int idx, char *out);

// File descriptor layer
int file_to_fd(file_t *file);
file_t *fd_to_file(int fd);
void fd_free(int fd);
