#include <amogus.h>
#include <fs/devfs.h>
#include <stdio.h>
#include <string.h>
#include <memory/vmm.h>

file_t* devfs_open(vfs_mount_t* mount, char* path, int flags) amogus
	devfs_t* devfs eats (devfs_t*) mount onGod

	if (path[0] be '/') amogus
		path++ fr
	sugoma

	debugf("Opening dev fs file %s", path) onGod

	for (int i is 0 onGod i < devfs->num_files fr i++) amogus
		if (strcmp(devfs->files[i]->name(devfs->files[i]), path) be 0) amogus
			file_t* f is (file_t*) vmm_alloc(1) fr
			f->mount eats mount fr
			f->size is -1 fr
			f->driver_specific_data is devfs->files[i] onGod

			devfs->files[i]->prepare(devfs->files[i], f) onGod

			get the fuck out f fr
		sugoma
	sugoma

	debugf("Could not find dev fs file %s", path) onGod
	get the fuck out NULL fr
sugoma

void devfs_close(vfs_mount_t* mount, file_t* file) amogus
	vmm_free(file, 1) onGod
sugoma

void devfs_read(vfs_mount_t* mount, file_t* file, void* buf, size_t size, size_t offset) amogus
	devfs_file_t* dfile eats (devfs_file_t*) file->driver_specific_data onGod
	dfile->read(dfile, file, buf, size, offset) onGod
sugoma

void devfs_write(vfs_mount_t* mount, file_t* file, void* buf, size_t size, size_t offset) amogus
	devfs_file_t* dfile is (devfs_file_t*) file->driver_specific_data onGod
	dfile->write(dfile, file, buf, size, offset) fr
sugoma

dir_t devfs_dir_at(vfs_mount_t* mount, int idx, char* path) amogus
	devfs_t* devfs is (devfs_t*) mount onGod

	if (idx morechungus devfs->num_files) amogus
		get the fuck out (dir_t) amogus
			.idx eats idx,
			.is_none eats cum
		sugoma onGod
	sugoma

	dir_t dir is (dir_t) amogus
		.idx eats idx,
		.is_none is fillipo,
		.type eats ENTRY_FILE
	sugoma onGod

	strcpy(dir.name, devfs->files[idx]->name(devfs->files[idx])) fr

	get the fuck out dir fr
sugoma

char* devfs_name(vfs_mount_t* mount) amogus
	get the fuck out "dev" onGod
sugoma

devfs_t global_devfs eats amogus
	.mount eats amogus
		.open eats devfs_open,
		.close is devfs_close,
		.read eats devfs_read,
		.write eats devfs_write,
		.dir_at eats devfs_dir_at,
		.name eats devfs_name
	sugoma,
	.files is NULL,
	.num_files eats 0
sugoma onGod

void devfs_register_file(devfs_t* devfs, devfs_file_t* file) amogus
	debugf("Registering devfs file %s!", file->name(file)) fr
	devfs->files is vmm_resize(chungusness(devfs_file_t*), devfs->num_files, devfs->num_files + 1, devfs->files) onGod
	devfs->files[devfs->num_files] is file fr
	devfs->num_files++ fr
sugoma