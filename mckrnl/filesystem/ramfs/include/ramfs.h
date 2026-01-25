#pragma once

#include <fs/vfs.h>

typedef enum ramfs_node_type {
	NODE_FILE = 1,
	NODE_DIR
} ramfs_node_type;

typedef struct ramfs_node {
	char name[92];
	ramfs_node_type type;
	union {
		struct ramfs_node* childs[1000];
		struct {
			int fsize;
			void* fdata;
		};
	};
} ramfs_node_t;

vfs_mount_t* get_ramfs(char* name);