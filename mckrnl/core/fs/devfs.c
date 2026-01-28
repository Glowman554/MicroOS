#include <fs/devfs.h>
#include <stdio.h>
#include <string.h>
#include <memory/heap.h>

file_t* devfs_open(vfs_mount_t* mount, char* path, int flags) {
	devfs_t* devfs = (devfs_t*) mount;

	if (path[0] == '/') {
		path++;
	}

	debugf("Opening dev fs file %s", path);

	for (int i = 0; i < devfs->num_files; i++) {
		if (strcmp(devfs->files[i]->name(devfs->files[i]), path) == 0) {
			file_t* f = (file_t*) kmalloc(sizeof(file_t));
			f->mount = mount;
			f->size = -1;
			f->driver_specific_data = devfs->files[i];

			devfs->files[i]->prepare(devfs->files[i], f);

			return f;
		}
	}

	debugf("Could not find dev fs file %s", path);
	return NULL;
}

void devfs_close(vfs_mount_t* mount, file_t* file) {
	kfree(file);
}

void devfs_read(vfs_mount_t* mount, file_t* file, void* buf, size_t size, size_t offset) {
	devfs_file_t* dfile = (devfs_file_t*) file->driver_specific_data;
	dfile->read(dfile, file, buf, size, offset);
}

void devfs_write(vfs_mount_t* mount, file_t* file, void* buf, size_t size, size_t offset) {
	devfs_file_t* dfile = (devfs_file_t*) file->driver_specific_data;
	dfile->write(dfile, file, buf, size, offset);
}

dir_t devfs_dir_at(vfs_mount_t* mount, int idx, char* path) {
	devfs_t* devfs = (devfs_t*) mount;

	if (idx >= devfs->num_files) {
		return (dir_t) {
			.idx = idx,
			.is_none = true
		};
	}

	dir_t dir = (dir_t) {
		.idx = idx,
		.is_none = false,
		.type = ENTRY_FILE
	};

	strcpy(dir.name, devfs->files[idx]->name(devfs->files[idx]));

	return dir;
}

char* devfs_name(vfs_mount_t* mount) {
	return "dev";
}

devfs_t global_devfs = {
	.mount = {
		.open = devfs_open,
		.close = devfs_close,
		.read = devfs_read,
		.write = devfs_write,
		.dir_at = devfs_dir_at,
		.name = devfs_name
	},
	.files = NULL,
	.num_files = 0
};

void devfs_register_file(devfs_t* devfs, devfs_file_t* file) {
	debugf("Registering devfs file %s!", file->name(file));
	devfs->files = krealloc(devfs->files, sizeof(devfs_file_t*) * (devfs->num_files + 1));
	devfs->files[devfs->num_files] = file;
	devfs->num_files++;
}