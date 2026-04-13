#include <fs/vfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

vfs_mount_t *mounts[VFS_MAX_MOUNTS];
int num_mounts = 0;

file_t *fd_table[VFS_MAX_FDS];

void vfs_init(void) {
    num_mounts = 0;
    memset(fd_table, 0, sizeof(fd_table));
}

void vfs_mount(vfs_mount_t *mount) {
    if (num_mounts < VFS_MAX_MOUNTS) {
        mounts[num_mounts++] = mount;
    }
}

vfs_mount_t *vfs_find_mount(char *path, char *file_path_out) {
    char buf[512];
    strncpy(buf, path, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    for (int i = 0; buf[i]; i++) {
        if (buf[i] == ':') {
            buf[i] = '\0';
            strcpy(file_path_out, &buf[i + 1]);

            for (int j = 0; j < num_mounts; j++) {
                if (strcmp(mounts[j]->name(mounts[j]), buf) == 0) {
                    return mounts[j];
                }
            }
            return NULL;
        }
    }
    return NULL;
}

file_t *vfs_open(char *path, int flags) {
    char file_path[512];
    vfs_mount_t *m = vfs_find_mount(path, file_path);
    if (!m || !m->open) {
        return NULL;
    }

    file_t *f = m->open(m, file_path, flags);
    if (f) {
        f->mount = m;
    }

    return f;
}

void vfs_close(file_t *file) {
    if (file && file->mount && file->mount->close) {
        file->mount->close(file->mount, file);
    }
}

void vfs_read(file_t *file, void *buf, size_t size, size_t offset) {
    if (file && file->mount && file->mount->read) {
        file->mount->read(file->mount, file, buf, size, offset);
    }
}

void vfs_write(file_t *file, void *buf, size_t size, size_t offset) {
    if (file && file->mount && file->mount->write) {
        file->mount->write(file->mount, file, buf, size, offset);
    }
}

void vfs_delete(file_t *file) {
    if (file && file->mount && file->mount->_delete) {
        file->mount->_delete(file->mount, file);
    }
}

void vfs_truncate(file_t *file, size_t new_size) {
    if (file && file->mount && file->mount->truncate) {
        file->mount->truncate(file->mount, file, new_size);
    }
}

void vfs_mkdir(char *path) {
    char file_path[512];
    vfs_mount_t *m = vfs_find_mount(path, file_path);
    if (m && m->mkdir) {
        m->mkdir(m, file_path);
    }
}

void vfs_touch(char *path) {
    char file_path[512];
    vfs_mount_t *m = vfs_find_mount(path, file_path);
    if (m && m->touch) {
        m->touch(m, file_path);
    }
}

dir_t vfs_dir_at(int idx, char *path) {
    char file_path[512];
    dir_t d = { .is_none = true };
    vfs_mount_t *m = vfs_find_mount(path, file_path);
    if (m && m->dir_at) {
        d = m->dir_at(m, idx, file_path);
    }
    return d;
}

void vfs_delete_dir(char *path) {
    char file_path[512];
    vfs_mount_t *m = vfs_find_mount(path, file_path);
    if (m && m->delete_dir) {
        m->delete_dir(m, file_path);
    }
}

bool vfs_fs_at(int idx, char *out) {
    if (idx < 0 || idx >= num_mounts) {
        return false;
    }

    strcpy(out, mounts[idx]->name(mounts[idx]));
    return true;
}

int file_to_fd(file_t *file) {
    for (int i = 0; i < VFS_MAX_FDS; i++) {
        if (!fd_table[i]) {
            fd_table[i] = file;
            return i + VFS_FD_OFFSET;
        }
    }
    return -1;
}

file_t *fd_to_file(int fd) {
    int idx = fd - VFS_FD_OFFSET;
    if (idx < 0 || idx >= VFS_MAX_FDS) {
        return NULL;
    }
    return fd_table[idx];
}

void fd_free(int fd) {
    int idx = fd - VFS_FD_OFFSET;
    if (idx >= 0 && idx < VFS_MAX_FDS) {
        fd_table[idx] = NULL;
    }
}
