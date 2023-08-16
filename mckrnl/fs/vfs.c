#include <fs/vfs.h>

#include <utils/string.h>
#include <stdio.h>
#include <config.h>
#include <driver/disk_driver.h>

#include <memory/vmm.h>
#include <stddef.h>

vfs_mount_t** vfs_mounts = NULL;
int num_vfs_mounts = 0;
fs_scanner* vfs_scanner = NULL;
int num_vfs_scanners = 0;

void vfs_init() {
	debugf("VFS: Initializing VFS");
}

bool try_read_disk_label(char* out, vfs_mount_t* mount) {
	file_t* label = mount->open(mount, "/LABEL", FILE_OPEN_MODE_READ);
	if (!label) {
		label = mount->open(mount, "/FOXCFG/dn.fox", FILE_OPEN_MODE_READ);
	}
	if (label) {
		mount->read(mount, label, out, label->size, 0);
		out[label->size] = 0;
		mount->close(mount, label);
		return true;
	} else {
		debugf("Could not read disk label for disk %s", mount->name(mount));
	}
	return false;
}

void vfs_mount(vfs_mount_t* mount) {
	debugf("VFS: Mounting %s", mount->name(mount));

	vfs_mounts = vmm_resize(sizeof(vfs_mount_t*), num_vfs_mounts, num_vfs_mounts + 1, vfs_mounts);
	vfs_mounts[num_vfs_mounts] = mount;
	num_vfs_mounts++;
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

			for (int j = 0; j < num_vfs_mounts; j++) {
				if (strcmp(vfs_mounts[j]->name(vfs_mounts[j]), _filename) == 0) {
					mount = vfs_mounts[j];
					break;
				}
			}
			break;
		}
	}

	if (mount == NULL) {
		debugf("No device found for path: %s", path);
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

			for (int j = 0; j < num_vfs_mounts; j++) {
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

			for (int j = 0; j < num_vfs_mounts; j++) {
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

			for (int j = 0; j < num_vfs_mounts; j++) {
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

			for (int j = 0; j < num_vfs_mounts; j++) {
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
	if (idx >= num_vfs_mounts) {
		return false;
	}

	strcpy(out, vfs_mounts[idx]->name(vfs_mounts[idx]));
	
	return true;
}

void vfs_register_fs_scanner(fs_scanner scanner) {
	vfs_scanner = vmm_resize(sizeof(fs_scanner), num_vfs_scanners, num_vfs_scanners + 1, vfs_scanner);
	vfs_scanner[num_vfs_scanners] = scanner;
	num_vfs_scanners++;
}

void vfs_scan_fs() {
	for (int i = 0; i < num_disks; i++) {
		for (int j = 0; j < num_vfs_scanners; j++) {
			vfs_mount_t* mount = vfs_scanner[j](i);
			if (mount != NULL) {
				vfs_mount(mount);
				break;
			}
		}
	}
}