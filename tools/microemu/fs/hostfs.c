#include <fs/hostfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

void hostfs_resolve(hostfs_t *hfs, const char *vpath, char *out, size_t out_size) {
    snprintf(out, out_size, "%s%s", hfs->host_root, vpath);
}

file_t *hostfs_open(vfs_mount_t *mount, char *path, int flags) {
    hostfs_t *hfs = (hostfs_t *)mount;
    char resolved[1024];
    hostfs_resolve(hfs, path, resolved, sizeof(resolved));

    struct stat st;
    if (stat(resolved, &st) == 0 && S_ISDIR(st.st_mode)) {
        return NULL;
    }

    int oflags = O_RDONLY;
    if (flags == FILE_OPEN_MODE_WRITE) {
        oflags = O_WRONLY | O_CREAT;
    } else if (flags == FILE_OPEN_MODE_READ_WRITE) {
        oflags = O_RDWR | O_CREAT;
    }

    int fd = open(resolved, oflags, 0644);
    if (fd < 0) {
        return NULL;
    }

    file_t *f = calloc(1, sizeof(file_t));
    f->mount = mount;
    f->mode = flags;
    f->driver_specific_data = (void *)(intptr_t)fd;
    strncpy(f->path, path, sizeof(f->path) - 1);

    if (fstat(fd, &st) == 0) {
        f->size = st.st_size;
    }

    return f;
}

void hostfs_close(vfs_mount_t *mount, file_t *file) {
    int fd = (int)(intptr_t)file->driver_specific_data;
    close(fd);
    free(file);
}

void hostfs_read(vfs_mount_t *mount, file_t *file, void *buf, size_t size, size_t offset) {
    int fd = (int)(intptr_t)file->driver_specific_data;
    pread(fd, buf, size, offset);
}

void hostfs_write(vfs_mount_t *mount, file_t *file, void *buf, size_t size, size_t offset) {
    int fd = (int)(intptr_t)file->driver_specific_data;
    pwrite(fd, buf, size, offset);
}

void hostfs_delete(vfs_mount_t *mount, file_t *file) {
    hostfs_t *hfs = (hostfs_t *)mount;
    char resolved[1024];
    hostfs_resolve(hfs, file->path, resolved, sizeof(resolved));
    int fd = (int)(intptr_t)file->driver_specific_data;
    close(fd);
    unlink(resolved);
    free(file);
}

void hostfs_mkdir(vfs_mount_t *mount, char *path) {
    hostfs_t *hfs = (hostfs_t *)mount;
    char resolved[1024];
    hostfs_resolve(hfs, path, resolved, sizeof(resolved));
    mkdir(resolved, 0755);
}

void hostfs_touch(vfs_mount_t *mount, char *path) {
    hostfs_t *hfs = (hostfs_t *)mount;
    char resolved[1024];
    hostfs_resolve(hfs, path, resolved, sizeof(resolved));
    int fd = open(resolved, O_CREAT | O_WRONLY, 0644);
    if (fd >= 0) {
        close(fd);
    }
}

dir_t hostfs_dir_at(vfs_mount_t *mount, int idx, char *path) {
    hostfs_t *hfs = (hostfs_t *)mount;
    char resolved[1024];
    hostfs_resolve(hfs, path, resolved, sizeof(resolved));

    dir_t d = { .is_none = true };

    DIR *dp = opendir(resolved);
    if (!dp) {
        return d;
    }

    struct dirent *ent;
    int i = 0;
    while ((ent = readdir(dp)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        if (i == idx) {
            strncpy(d.name, ent->d_name, 256);
            d.name[255] = '\0';
            d.idx = idx;
            d.is_none = false;
            d.type = (ent->d_type == DT_DIR) ? ENTRY_DIR : ENTRY_FILE;
            break;
        }
        i++;
    }
    closedir(dp);
    return d;
}

void hostfs_delete_dir(vfs_mount_t *mount, char *path) {
    hostfs_t *hfs = (hostfs_t *)mount;
    char resolved[1024];
    hostfs_resolve(hfs, path, resolved, sizeof(resolved));
    rmdir(resolved);
}

void hostfs_truncate(vfs_mount_t *mount, file_t *file, size_t new_size) {
    int fd = (int)(intptr_t)file->driver_specific_data;
    ftruncate(fd, new_size);
    file->size = new_size;
}

char *hostfs_name(vfs_mount_t *mount) {
    hostfs_t *hfs = (hostfs_t *)mount;
    return hfs->mount_name;
}

hostfs_t *hostfs_create(const char *mount_name, const char *host_dir) {
    hostfs_t *hfs = calloc(1, sizeof(hostfs_t));
    strncpy(hfs->mount_name, mount_name, sizeof(hfs->mount_name) - 1);
    strncpy(hfs->host_root, host_dir, sizeof(hfs->host_root) - 1);

    hfs->mount.open = hostfs_open;
    hfs->mount.close = hostfs_close;
    hfs->mount.read = hostfs_read;
    hfs->mount.write = hostfs_write;
    hfs->mount._delete = hostfs_delete;
    hfs->mount.mkdir = hostfs_mkdir;
    hfs->mount.touch = hostfs_touch;
    hfs->mount.dir_at = hostfs_dir_at;
    hfs->mount.delete_dir = hostfs_delete_dir;
    hfs->mount.truncate = hostfs_truncate;
    hfs->mount.name = hostfs_name;

    return hfs;
}
