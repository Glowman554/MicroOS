#pragma once

#include <stddef.h>
#include <stdbool.h>

struct file;
struct dir_t;

enum file_open_mode_t {
	FILE_OPEN_MODE_READ,
	FILE_OPEN_MODE_WRITE,
	FILE_OPEN_MODE_READ_WRITE
};

typedef struct vfs_mount {
	struct file* (*open)(struct vfs_mount* mount, char* path, int flags);
	void (*close)(struct vfs_mount* mount, struct file* file);
	void (*read)(struct vfs_mount* mount, struct file* file, void* buf, size_t size, size_t offset);
	void (*write)(struct vfs_mount* mount, struct file* file, void* buf, size_t size, size_t offset);
	void (*_delete)(struct vfs_mount* mount, struct file* file);
	void (*mkdir)(struct vfs_mount* mount, char* path);
	void (*touch)(struct vfs_mount* mount, char* path);

	struct dir_t (*dir_at)(struct vfs_mount* mount, int idx, char* path);
	void (*delete_dir)(struct vfs_mount* mount, char* path);

	void (*truncate)(struct vfs_mount* mount, struct file* file, size_t new_size);

	char* (*name)(struct vfs_mount* mount);

	void* driver_specific_data;

} vfs_mount_t;

typedef struct file {
	vfs_mount_t* mount;
	int mode;
	void* driver_specific_data;
	char buffer[512];
	size_t size;
} file_t;

enum dir_entry_type_e {
	ENTRY_FILE,
	ENTRY_DIR
};

typedef struct dir_t {
	char name[256];
	int idx;
	bool is_none;
	int type;
} dir_t;

void vfs_init();

void vfs_mount(vfs_mount_t* mount);

file_t* vfs_open(char* path, int flags);
void vfs_close(file_t* file);
void vfs_read(file_t* file, void* buf, size_t size, size_t offset);
void vfs_write(file_t* file, void* buf, size_t size, size_t offset);
void vfs_delete(file_t* file);
void vfs_mkdir(char* path);
void vfs_touch(char* path);

void vfs_truncate(file_t* file, size_t new_size);

dir_t vfs_dir_at(int idx, char* path);
void vfs_delete_dir(char* path);

bool vfs_fs_at(int idx, char* output);

typedef vfs_mount_t* (*fs_scanner)(int disk_id);
void vfs_register_fs_scanner(fs_scanner scanner);
void vfs_scan_fs();

bool try_read_disk_label(char* output, vfs_mount_t* mount);