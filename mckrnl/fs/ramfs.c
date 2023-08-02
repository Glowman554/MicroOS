#include <fs/ramfs.h>

#include <memory/vmm.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

char* ramfs_name(vfs_mount_t* mount) {
	return (char*) (mount + sizeof(vfs_mount_t));
}

ramfs_node_t* ramfs_find(char* path, ramfs_node_t* current) {
	debugf("ramfs_find(%s, %x, %s)", path, current, current->name);

	while (*path == '/') {
		path++;
	}
	if (*path == 0) {
		return current;
	}

	char buffer[128] = { 0 };
	char* next = copy_until('/', path, buffer);

	for (int i = 0; i < sizeof(current->childs) / sizeof(current->childs[0]); i++) {
		if (current->childs[i] && strcasecmp(buffer, current->childs[i]->name) == 0) {
			return ramfs_find(next, current->childs[i]);
		}
	}

	return NULL;
}

char* ramfs_split_path(char* path, char* out) {
	char buffer[128] = { 0 };
	char* next = path;
	char* to_create;
	while (*(next = copy_until('/', next, buffer))) {
		to_create = next;
	}

	memcpy(out, path, to_create - path - 1);

	return to_create;
}

file_t* ramfs_open(vfs_mount_t* mount, char* path, int flags) {
	ramfs_node_t* node = ramfs_find(path, mount->driver_specific_data);
	if (node == NULL || node->type != NODE_FILE) {
		return NULL;
	}

	file_t* f = (file_t*) vmm_alloc(1);
	f->mount = mount;
	f->size = node->fsize;
	f->driver_specific_data = node;

	return f;
}


void ramfs_close(vfs_mount_t* mount, file_t* file) {
	vmm_free(file, 1);
}

void ramfs_read(vfs_mount_t* mount, file_t* file, void* buf, size_t size, size_t offset) {
	ramfs_node_t* node = (ramfs_node_t*) file->driver_specific_data;

	assert(node->fsize >= offset + size);

	memcpy(buf, node->fdata + offset, size);
}

void ramfs_write(vfs_mount_t* mount, file_t* file, void* buf, size_t size, size_t offset) {
	ramfs_node_t* node = (ramfs_node_t*) file->driver_specific_data;

	int new_file_size = offset + size;
	if (new_file_size > node->fsize) {
		node->fdata = vmm_resize(1, node->fsize, new_file_size, node->fdata);
		node->fsize = new_file_size;
	}

	memcpy(node->fdata + offset, buf, size);
}


void ramfs_delete(vfs_mount_t* mount, file_t* file) { todo(); }

void ramfs_mkdir(vfs_mount_t* mount, char* path) {
	if (ramfs_find(path, mount->driver_specific_data)) {
		debugf("%s ALREADY EXISTS", path);
		return;
	}

	char path_buf[512] = { 0 };
	char* to_create = ramfs_split_path(path, path_buf);

	ramfs_node_t* node = ramfs_find(path_buf, mount->driver_specific_data);
	if (node == NULL || node->type != NODE_DIR) {
		debugf("Node not found or of wrong type!");
		return;
	}

	for (int i = 0; i < sizeof(node->childs) / sizeof(node->childs[0]); i++) {
		if (!node->childs[i]) {
			ramfs_node_t* child = (ramfs_node_t*) vmm_alloc(1);
			memset(child, 0, sizeof(ramfs_node_t));
			child->type = NODE_DIR;
			strcpy(child->name, to_create);

			node->childs[i] = child;
			return;
		}
	}
}

void ramfs_touch(vfs_mount_t* mount, char* path) {
	if (ramfs_find(path, mount->driver_specific_data)) {
		debugf("%s ALREADY EXISTS", path);
		return;
	}

	char path_buf[512] = { 0 };
	char* to_create = ramfs_split_path(path, path_buf);

	ramfs_node_t* node = ramfs_find(path_buf, mount->driver_specific_data);
	if (node == NULL || node->type != NODE_DIR) {
		debugf("Node not found or of wrong type!");
		return;
	}

	for (int i = 0; i < sizeof(node->childs) / sizeof(node->childs[0]); i++) {
		if (!node->childs[i]) {
			ramfs_node_t* child = (ramfs_node_t*) vmm_alloc(1);
			memset(child, 0, sizeof(ramfs_node_t));
			child->type = NODE_FILE;
			strcpy(child->name, to_create);

			node->childs[i] = child;
			return;
		}
	}
}

dir_t ramfs_dir_at(vfs_mount_t* mount, int idx, char* path) {
	ramfs_node_t* node = ramfs_find(path, mount->driver_specific_data);
	if (node == NULL || node->type != NODE_DIR) {
		return (dir_t) { .is_none = true };
	}

	int actual_idx = 0;
	int current_idx = idx + 1;
	while (true) {
		if (node->childs[actual_idx]) {
			current_idx--;
		}
		if (!current_idx) {
			break;
		} else {
			actual_idx++;
			if (actual_idx >= sizeof(node->childs) / sizeof(node->childs[0])) {
				return (dir_t) { .is_none = true };
			}
		}
	}

	dir_t dir = { 0 };
	dir.idx = idx;
	dir.is_none = false;

	switch (node->childs[actual_idx]->type) {
		case NODE_DIR:
			dir.type = ENTRY_DIR;
			break;
		case NODE_FILE:
			dir.type = ENTRY_FILE;
			break;
		default:
			return (dir_t) { .is_none = true };
	}

	strcpy(dir.name, node->childs[actual_idx]->name);

	return dir;
}


void ramfs_delete_dir(vfs_mount_t* mount, char* path) { todo(); }


vfs_mount_t* get_ramfs(char* name) {
	assert(sizeof(ramfs_node_t) == 0x1000);
	vfs_mount_t* mount = (vfs_mount_t*) vmm_alloc(1);
	memset(mount, 0, 0x1000);

	mount->open = ramfs_open;
	mount->close = ramfs_close;
	mount->read = ramfs_read;
	mount->write = ramfs_write;
	mount->_delete = ramfs_delete;
	mount->mkdir = ramfs_mkdir;
	mount->touch = ramfs_touch;

	mount->dir_at = ramfs_dir_at;
	mount->delete_dir = ramfs_delete_dir;

	mount->name = ramfs_name;

	strcpy((char*) (mount + sizeof(vfs_mount_t)), name);

	ramfs_node_t* root = (ramfs_node_t*) vmm_alloc(1);
	memset(root, 0, sizeof(ramfs_node_t));
	root->type = NODE_DIR;

	mount->driver_specific_data = root;
	
	return mount;
}