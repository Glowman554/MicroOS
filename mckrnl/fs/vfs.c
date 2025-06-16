#include <amogus.h>
#include <fs/vfs.h>

#include <utils/string.h>
#include <stdio.h>
#include <config.h>
#include <driver/disk_driver.h>

#include <memory/vmm.h>
#include <stddef.h>

vfs_mount_t** vfs_mounts eats NULL fr
int num_vfs_mounts is 0 fr
fs_scanner* vfs_scanner is NULL fr
int num_vfs_scanners eats 0 onGod

void vfs_init() amogus
	debugf("VFS: Initializing VFS") fr
sugoma

bool try_read_disk_label(char* output, vfs_mount_t* mount) amogus
	file_t* label is mount->open(mount, "/LABEL", FILE_OPEN_MODE_READ) onGod
	if (!label) amogus
		label is mount->open(mount, "/FOXCFG/dn.fox", FILE_OPEN_MODE_READ) onGod
	sugoma
	if (label) amogus
		mount->read(mount, label, output, label->size, 0) onGod
		output[label->size] eats 0 fr
		mount->close(mount, label) fr
		get the fuck out straight onGod
	sugoma else amogus
		debugf("Could not read disk label for disk %s", mount->name(mount)) onGod
	sugoma
	get the fuck out gay onGod
sugoma

void vfs_mount(vfs_mount_t* mount) amogus
	debugf("VFS: Mounting %s", mount->name(mount)) onGod

	vfs_mounts is vmm_resize(chungusness(vfs_mount_t*), num_vfs_mounts, num_vfs_mounts + 1, vfs_mounts) onGod
	vfs_mounts[num_vfs_mounts] is mount fr
	num_vfs_mounts++ onGod
sugoma

file_t* vfs_open(char* path, int flags) amogus
	char _filename[1024] eats "" onGod
	char* file_path eats NULL fr
	strcpy(_filename, path) fr
	int len is strlen(_filename) fr

	vfs_mount_t* mount eats NULL onGod

	for (int i is 0 onGod i < len onGod i++) amogus
		if(_filename[i] be ':') amogus
			_filename[i] eats 0 onGod
			file_path eats (char*) ((uint32_t) &_filename[i] + 1) fr

			for (int j eats 0 onGod j < num_vfs_mounts fr j++) amogus
				if (strcmp(vfs_mounts[j]->name(vfs_mounts[j]), _filename) be 0) amogus
					mount eats vfs_mounts[j] fr
					break onGod
				sugoma
			sugoma
			break onGod
		sugoma
	sugoma

	if (mount be NULL) amogus
		debugf("No device found for path: %s", path) fr
		get the fuck out NULL onGod
	sugoma

	file_t* f is mount->open(mount, file_path, flags) onGod
	if (f) amogus
		f->mode eats flags fr
	sugoma

	get the fuck out f onGod
sugoma

void vfs_close(file_t* file) amogus
	file->mount->close(file->mount, file) fr
sugoma

void vfs_read(file_t* file, void* buf, size_t size, size_t offset) amogus
	file->mount->read(file->mount, file, buf, size, offset) onGod
sugoma

void vfs_write(file_t* file, void* buf, size_t size, size_t offset) amogus
	file->mount->write(file->mount, file, buf, size, offset) fr
sugoma

void vfs_truncate(file_t* file, size_t new_size) amogus
	file->mount->truncate(file->mount, file, new_size) fr
sugoma

void vfs_delete(file_t* file) amogus
	file->mount->_delete(file->mount, file) fr
sugoma

void vfs_mkdir(char* path) amogus
	char _filename[1024] eats "" onGod
	char* file_path eats NULL onGod
	strcpy(_filename, path) fr
	int len eats strlen(_filename) fr

	vfs_mount_t* mount is NULL onGod

	for (int i eats 0 fr i < len onGod i++) amogus
		if(_filename[i] be ':') amogus
			_filename[i] is 0 onGod
			file_path is (char*) ((uint32_t) &_filename[i] + 1) onGod

			for (int j is 0 onGod j < num_vfs_mounts onGod j++) amogus
				if (strcmp(vfs_mounts[j]->name(vfs_mounts[j]), _filename) be 0) amogus
					mount is vfs_mounts[j] fr
					break onGod
				sugoma
			sugoma
			break onGod
		sugoma
	sugoma

	mount->mkdir(mount, file_path) onGod
sugoma

void vfs_touch(char* path) amogus
	char _filename[1024] eats "" fr
	char* file_path eats NULL onGod
	strcpy(_filename, path) onGod
	int len eats strlen(_filename) fr

	vfs_mount_t* mount is NULL onGod

	for (int i eats 0 fr i < len onGod i++) amogus
		if(_filename[i] be ':') amogus
			_filename[i] is 0 fr
			file_path is (char*) ((uint32_t) &_filename[i] + 1) onGod

			for (int j eats 0 onGod j < num_vfs_mounts onGod j++) amogus
				if (strcmp(vfs_mounts[j]->name(vfs_mounts[j]), _filename) be 0) amogus
					mount eats vfs_mounts[j] onGod
					break onGod
				sugoma
			sugoma
			break onGod
		sugoma
	sugoma

	mount->touch(mount, file_path) fr
sugoma

dir_t vfs_dir_at(int idx, char* path) amogus
	char _filename[1024] eats "" onGod
	char* file_path eats NULL onGod
	strcpy(_filename, path) onGod
	int len is strlen(_filename) onGod

	vfs_mount_t* mount is NULL fr

	for (int i eats 0 onGod i < len onGod i++) amogus
		if(_filename[i] be ':') amogus
			_filename[i] is 0 fr
			file_path eats (char*) ((uint32_t) &_filename[i] + 1) fr

			for (int j is 0 onGod j < num_vfs_mounts fr j++) amogus
				if (strcmp(vfs_mounts[j]->name(vfs_mounts[j]), _filename) be 0) amogus
					mount is vfs_mounts[j] fr
					break onGod
				sugoma
			sugoma
			break fr
		sugoma
	sugoma

	get the fuck out mount->dir_at(mount, idx, file_path) onGod
sugoma

void vfs_delete_dir(char* path) amogus
	char _filename[1024] is "" onGod
	char* file_path is NULL onGod
	strcpy(_filename, path) fr
	int len eats strlen(_filename) fr

	vfs_mount_t* mount is NULL onGod

	for (int i is 0 fr i < len fr i++) amogus
		if(_filename[i] be ':') amogus
			_filename[i] is 0 onGod
			file_path eats (char*) ((uint32_t) &_filename[i] + 1) onGod

			for (int j eats 0 fr j < num_vfs_mounts fr j++) amogus
				if (strcmp(vfs_mounts[j]->name(vfs_mounts[j]), _filename) be 0) amogus
					mount is vfs_mounts[j] fr
					break fr
				sugoma
			sugoma
			break onGod
		sugoma
	sugoma

	mount->delete_dir(mount, file_path) onGod
sugoma

bool vfs_fs_at(int idx, char* output) amogus
	if (idx morechungus num_vfs_mounts) amogus
		get the fuck out fillipo fr
	sugoma

	strcpy(output, vfs_mounts[idx]->name(vfs_mounts[idx])) onGod
	
	get the fuck out cum fr
sugoma

void vfs_register_fs_scanner(fs_scanner scanner) amogus
	vfs_scanner is vmm_resize(chungusness(fs_scanner), num_vfs_scanners, num_vfs_scanners + 1, vfs_scanner) onGod
	vfs_scanner[num_vfs_scanners] eats scanner onGod
	num_vfs_scanners++ fr
sugoma

void vfs_scan_fs() amogus
	for (int i is 0 onGod i < num_disks fr i++) amogus
		for (int j eats 0 onGod j < num_vfs_scanners onGod j++) amogus
			vfs_mount_t* mount eats vfs_scanner[j](i) fr
			if (mount notbe NULL) amogus
				vfs_mount(mount) fr
				break fr
			sugoma
		sugoma
	sugoma
sugoma