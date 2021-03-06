#include <fs/vfs.h>

#include <utils/string.h>
#include <stdio.h>
#include <config.h>
#include <driver/disk_driver.h>

vfs_mount_t* vfs_mounts[MAX_VFS_MOUNTS] = { 0 };
fs_scanner vfs_scanner[MAX_VFS_SCANNERS] = { 0 };

void vfs_init() {
	debugf("VFS: Initializing VFS");
	memset(vfs_mounts, 0, sizeof(vfs_mounts));
	memset(vfs_scanner, 0, sizeof(vfs_scanner));
}

void vfs_mount(vfs_mount_t* mount) {
	debugf("VFS: Mounting %s", mount->name(mount));

	for (int i = 0; i < MAX_VFS_MOUNTS; i++) {
		if (vfs_mounts[i] == 0) {
			vfs_mounts[i] = mount;
			return;
		}
	}

	abortf("VFS: Error: No more mount points available\n");
}

file_t* vfs_open(char* path, int flags) {
	char _filename[1024] = "";
	char* file_path = NULL;
	strcpy(_filename, path);
	int len = strlen(_filename);

	vfs_mount_t* mount = NULL;

	for (int i = 0; i < len; i++) {
		if(_filename[i] == ':') {
			_filename[i] = 0;
			file_path = (char*) ((uint32_t) &_filename[i] + 1);

			for (int j = 0; j < MAX_VFS_MOUNTS; j++) {
				if (vfs_mounts[j] == NULL) {
					continue;
				}

				if (strcmp(vfs_mounts[j]->name(vfs_mounts[j]), _filename) == 0) {
					mount = vfs_mounts[j];
					break;
				}
			}
			break;
		}
	}

	if (mount == NULL) {
		debugf("No device found for path: %s\n", path);
		return NULL;
	}

	file_t* f = mount->open(mount, file_path, flags);
	if (f) {
		f->mode = flags;
	}

	return f;
}

void vfs_close(file_t* file) {
	file->mount->close(file->mount, file);
}

void vfs_read(file_t* file, void* buf, size_t size, size_t offset) {
	file->mount->read(file->mount, file, buf, size, offset);
}

void vfs_write(file_t* file, void* buf, size_t size, size_t offset) {
	file->mount->write(file->mount, file, buf, size, offset);
}

void vfs_delete(file_t* file) {
	file->mount->_delete(file->mount, file);
}

void vfs_mkdir(char* path) {
	char _filename[1024] = "";
	char* file_path = NULL;
	strcpy(_filename, path);
	int len = strlen(_filename);

	vfs_mount_t* mount = NULL;

	for (int i = 0; i < len; i++) {
		if(_filename[i] == ':') {
			_filename[i] = 0;
			file_path = (char*) ((uint32_t) &_filename[i] + 1);

			for (int j = 0; j < MAX_VFS_MOUNTS; j++) {
				if (vfs_mounts[j] == NULL) {
					continue;
				}
				
				if (strcmp(vfs_mounts[j]->name(vfs_mounts[j]), _filename) == 0) {
					mount = vfs_mounts[j];
					break;
				}
			}
			break;
		}
	}

	mount->mkdir(mount, file_path);
}

void vfs_touch(char* path) {
	char _filename[1024] = "";
	char* file_path = NULL;
	strcpy(_filename, path);
	int len = strlen(_filename);

	vfs_mount_t* mount = NULL;

	for (int i = 0; i < len; i++) {
		if(_filename[i] == ':') {
			_filename[i] = 0;
			file_path = (char*) ((uint32_t) &_filename[i] + 1);

			for (int j = 0; j < MAX_VFS_MOUNTS; j++) {
				if (vfs_mounts[j] == NULL) {
					continue;
				}

				if (strcmp(vfs_mounts[j]->name(vfs_mounts[j]), _filename) == 0) {
					mount = vfs_mounts[j];
					break;
				}
			}
			break;
		}
	}

	mount->touch(mount, file_path);
}

dir_t vfs_dir_at(int idx, char* path) {
	char _filename[1024] = "";
	char* file_path = NULL;
	strcpy(_filename, path);
	int len = strlen(_filename);

	vfs_mount_t* mount = NULL;

	for (int i = 0; i < len; i++) {
		if(_filename[i] == ':') {
			_filename[i] = 0;
			file_path = (char*) ((uint32_t) &_filename[i] + 1);

			for (int j = 0; j < MAX_VFS_MOUNTS; j++) {
				if (vfs_mounts[j] == NULL) {
					continue;
				}

				if (strcmp(vfs_mounts[j]->name(vfs_mounts[j]), _filename) == 0) {
					mount = vfs_mounts[j];
					break;
				}
			}
			break;
		}
	}

	return mount->dir_at(mount, idx, file_path);
}

void vfs_delete_dir(char* path) {
	char _filename[1024] = "";
	char* file_path = NULL;
	strcpy(_filename, path);
	int len = strlen(_filename);

	vfs_mount_t* mount = NULL;

	for (int i = 0; i < len; i++) {
		if(_filename[i] == ':') {
			_filename[i] = 0;
			file_path = (char*) ((uint32_t) &_filename[i] + 1);

			for (int j = 0; j < MAX_VFS_MOUNTS; j++) {
				if (vfs_mounts[j] == NULL) {
					continue;
				}
				
				if (strcmp(vfs_mounts[j]->name(vfs_mounts[j]), _filename) == 0) {
					mount = vfs_mounts[j];
					break;
				}
			}
			break;
		}
	}

	mount->delete_dir(mount, file_path);
}

bool vfs_fs_at(int idx, char* out) {
	if (idx >= MAX_VFS_MOUNTS) {
		return false;
	}

	if (vfs_mounts[idx] == 0) {
		return false;
	}

	strcpy(out, vfs_mounts[idx]->name(vfs_mounts[idx]));
	
	return true;
}

void vfs_register_fs_scanner(fs_scanner scanner) {
	for (int i = 0; i < MAX_VFS_SCANNERS; i++) {
		if (vfs_scanner[i] == NULL) {
			vfs_scanner[i] = scanner;
			return;
		}
	}

	abortf("No more space for fs scanners");
}

void vfs_scan_fs() {
	for (int i = 0; i < num_disks; i++) {
		for (int j = 0; j < MAX_VFS_SCANNERS; j++) {
			if (vfs_scanner[j] == NULL) {
				continue;
			}
			vfs_mount_t* mount = vfs_scanner[j](i);
			if (mount != NULL) {
				vfs_mount(mount);
				break;
			}
		}
	}
}