#include <fs/initrd.h>

#include <stdio.h>
#include <memory/vmm.h>
#include <utils/string.h>

char** initrd_process_path(char* path) {
	path++;
	char** path_parts = (char**) vmm_alloc(1);
	int num_slashes = 0;
	int len = strlen(path);

	char* last_slash = path;
	for (int i = 0; i < len; i++) {
		if (path[i] == '/') {
			path[i] = '\0';
			path_parts[num_slashes] = last_slash;
			num_slashes++;

			last_slash = &path[i + 1];
		}
	}

	path_parts[num_slashes] = last_slash;
	path_parts[num_slashes + 1] = NULL;

	return path_parts;
}

saf_node_hdr_t* initrd_resolve(saf_node_hdr_t* curr, void* saf_image, int level, char** path) {
	if(curr->magic != MAGIC_NUMBER) {
		debugf("curr->magic != MAGIC_NUMBER\n");
		return NULL;
	}

	// debugf("level %d (%s), curr->name %s\n", level,  level < 0 ? "-1" : path[level], curr->name);

	if (path[level + 1] == NULL) {
		for (int i = 0; i < strlen(path[level]); i++) {
			if (path[level][i] >= 'A' && path[level][i] <= 'Z') {
				path[level][i] = path[level][i] + 32;
			}
		}

		char curr_name_cpy[512] = {0};
		strcpy(curr_name_cpy, curr->name);

		for (int i = 0; i < strlen(curr_name_cpy); i++) {
			if (curr_name_cpy[i] >= 'A' && curr_name_cpy[i] <= 'Z') {
				curr_name_cpy[i] = curr_name_cpy[i] + 32;
			}
		}

		
		if (strcmp(curr_name_cpy, path[level]) == 0) {
			return curr;
		} else {
			return NULL;
		}
	} else {
		if (curr->flags != FLAG_ISFOLDER) {
			return NULL;
		} else {

			saf_node_folder_t* folder = (saf_node_folder_t*) curr;
			for (int i = 0; i < folder->num_children; i++) {
				saf_node_hdr_t* child = (saf_node_hdr_t*) ((uint32_t) saf_image + (uint32_t) folder->children[i]);
				if (child->magic == MAGIC_NUMBER) {
					saf_node_hdr_t* result = initrd_resolve(child, saf_image, level + 1, path);
					if (result != NULL) {
						return result;
					}
				}
			}

			return NULL;
		}
	}
}

char* initrd_name(vfs_mount_t* mount) {
	return "initrd";
}

file_t* initrd_open(vfs_mount_t* mount, char* path, int flags) {
	debugf("open: %s", path);
	char path_cpy[strlen(path) + 1];
	strcpy(path_cpy, path);
	path_cpy[strlen(path)] = 0;

	char** path_parts = initrd_process_path(path_cpy);

	saf_node_hdr_t* current_node = (saf_node_hdr_t*) mount->driver_specific_data;
	saf_node_hdr_t* file = initrd_resolve(current_node, mount->driver_specific_data, -1, path_parts);
	if (file == NULL) {
		debugf("file %s not found", path);
		vmm_free(path_parts, 1);
		return NULL;
	}

	if (file->flags == FLAG_ISFOLDER) {
		debugf("file %s is a folder", path);
		vmm_free(path_parts, 1);
		return NULL;
	}

	saf_node_file_t* file_node = (saf_node_file_t*) file;

	file_t* f = (file_t*) vmm_alloc(1);
	f->mount = mount;
	f->size = file_node->size;
	f->driver_specific_data = (void*) ((uint32_t) mount->driver_specific_data + (uint32_t) file_node->addr);

	vmm_free(path_parts, 1);

	return f;
}

void initrd_close(vfs_mount_t* mount, file_t* f) {
	vmm_free(f, 0);
}

void initrd_read(vfs_mount_t* mount, file_t* f, void* buffer, size_t size, size_t offset) {
	if (f->size >= offset + size) {
		debugf("read: %d bytes from %d", size, offset);
	}
	memcpy(buffer, (void*) ((uint32_t) f->driver_specific_data + offset), size);
}

dir_t initrd_dir_at(vfs_mount_t* mount, int idx, char* path) {
	debugf("dir_at: %s (%d)\n", path, idx);
	char path_cpy[strlen(path) + 1];
	strcpy(path_cpy, path);
	path_cpy[strlen(path)] = 0;;

	char** path_parts = initrd_process_path(path_cpy);

	saf_node_hdr_t* current_node = (saf_node_hdr_t*) mount->driver_specific_data;
	saf_node_hdr_t* folder;

	if (strcmp(path, (char*) "/") == 0 || strcmp(path, (char*) "") == 0) {
		folder = current_node;
	} else {
		folder = initrd_resolve(current_node, mount->driver_specific_data, -1, path_parts);
	}

	if (folder == NULL) {
		dir_t dir = {
			.is_none = false,
		};
		return dir;
	}

	if (folder->flags != FLAG_ISFOLDER) {
		dir_t dir = {
			.is_none = true,
		};
		return dir;
	}

	saf_node_folder_t* folder_node = (saf_node_folder_t*) folder;

	if (idx > folder_node->num_children - 1) {
		dir_t dir = {
			.is_none = true,
		};
		return dir;
	} else {
		saf_node_hdr_t* child = (saf_node_hdr_t*) ((uint32_t) mount->driver_specific_data + (uint32_t) folder_node->children[idx]);

		dir_t dir;
		memset(&dir, 0, sizeof(dir));

		strcpy(dir.name, child->name);
		dir.idx = idx;
		dir.is_none = false;

		if (child->flags == FLAG_ISFOLDER) {
			dir.type = ENTRY_DIR;
		} else {
			dir.type = ENTRY_FILE;
		}

		return dir;
	}
}

vfs_mount_t* initrd_mount(void* saf_image) {
	vfs_mount_t* mount = (vfs_mount_t*) vmm_alloc(1);
	memset(mount, 0, sizeof(vfs_mount_t));

	mount->driver_specific_data = saf_image;

	mount->name = initrd_name;
	mount->open = initrd_open;
	mount->close = initrd_close;
	mount->read = initrd_read;
	mount->dir_at = initrd_dir_at;

	return mount;
}