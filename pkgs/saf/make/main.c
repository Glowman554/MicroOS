#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <string.h>
#include <buildin/ansi.h>
#include <saf.h>

// port from https://github.com/chocabloc/saf/blob/main/saf-make.c

#define INITIAL_ALLOC_SIZE (32 * 1024 * 1024)
#define MAX_PATH_SIZE 1024

static bool is_quiet = false;

typedef struct {
	uint8_t* ptr;
	uint64_t curr_offset;
	uint64_t alloc_size;
} arch_data_t;

typedef struct {
	uint64_t len;
	saf_offset_t offsets[];
} nodelist_t;

static arch_data_t adata;

static void panic(const char* msg, ...) {
	char buf[512] = { 0 };
    ansi_printf("\033[31merror: \033[0m");
    va_list args;
	va_start(args, msg);
	vsprintf(buf, msg, args);
	va_end(args);
	fprintf(stderr, "%s\n", buf);
	exit(-1);
}

static void info(const char* msg, ...) {
	if (is_quiet) {
		return;
	}

	char buf[512] = { 0 };
    ansi_printf("\033[32minfo: \033[0m");
    va_list args;
	va_start(args, msg);
	vsprintf(buf, msg, args);
	va_end(args);
	printf("%s", buf);
}

static void inc_offset(uint64_t d) {
	adata.curr_offset += d;
    if (adata.curr_offset > adata.alloc_size) {
        panic("archive size exceeds %d bytes\n", adata.alloc_size);
	}
}

// count number of children of a directory
static uint64_t childcnt(const char* fldr) {
	char path[512] = { 0 };
	if (!resolve((char*) fldr, path)) {
		panic("could not resolve path %s\n", fldr);
	}

	uint64_t cnt = 0;
	dir_t dir;
    dir_at(path, cnt, &dir);
	while (!dir.is_none) {
        dir_at(path, ++cnt, &dir);
	}

	return cnt;
}

static nodelist_t* make_node(char* path) {
	uint64_t nc = childcnt(path);
	nodelist_t* ret = malloc(sizeof(nodelist_t) + (sizeof(saf_offset_t) * nc));
	ret->len = 0;

	char res_path[512] = { 0 };
	if (!resolve(path, res_path)) {
		panic("could not resolve path %s\n", path);
	}

	int idx = 0;
	dir_t ent;
    dir_at(res_path, idx, &ent);
	while (!ent.is_none) {
		char* fullpath = (char*)malloc(MAX_PATH_SIZE);
		sprintf(fullpath, "%s/%s", path, ent.name);
		info("adding node %s\n", ent.name);

		// ignore things that are not files or folders
		if (ent.type != ENTRY_FILE && ent.type != ENTRY_DIR) {
			info("ignoring node %s with unknown type\n", fullpath);
			continue;
		}

		ret->offsets[ret->len++] = adata.curr_offset;
		saf_node_hdr_t* hdr = (saf_node_hdr_t*)(adata.ptr + adata.curr_offset);
		memset(hdr, 0, sizeof(saf_node_hdr_t));
		hdr->magic = MAGIC_NUMBER;
		memcpy(&(hdr->name), &(ent.name), strlen(ent.name) + 1);

		// calculate size of node structure
		size_t nodesize;
		if (ent.type == ENTRY_DIR) {
			nodesize = sizeof(saf_node_folder_t) + (childcnt(fullpath) * sizeof(saf_offset_t));
		} else {
			nodesize = sizeof(saf_node_file_t);
		}
		inc_offset(nodesize);
		hdr->len = nodesize;

		// the node is a folder
		if (ent.type == ENTRY_DIR) {
			saf_node_folder_t* fldr = (saf_node_folder_t*)(hdr);
			fldr->hdr.flags = FLAG_ISFOLDER;
			nodelist_t* children = make_node(fullpath);
			fldr->num_children = children->len;
			for (size_t i = 0; i < children->len; i++) {
				fldr->children[i] = children->offsets[i];
			}
		}
		// the node is a file
		else if (ent.type == ENTRY_FILE) {
			saf_node_file_t* file = (saf_node_file_t*)(hdr);
			file->hdr.flags = 0;
			file->addr = adata.curr_offset;
			FILE* f = fopen(fullpath, "rb");
			if (!f) {
				panic("could not open file %s\n", fullpath);
			}
			fseek(f, 0, SEEK_END);
			file->size = ftell(f);
			fseek(f, 0, SEEK_SET);
			inc_offset(file->size);
			fread((uint8_t*)file + sizeof(saf_node_file_t), 1, file->size, f);
			fclose(f);
		}

        dir_at(res_path, ++idx, &ent);
	}
	return ret;
}

size_t calculate_size(char* path) {
	size_t size = 0;
	
	char res_path[512] = { 0 };
	if (!resolve(path, res_path)) {
		panic("could not resolve path %s\n", path);
	}

	int idx = 0;
	dir_t ent;
    dir_at(res_path, idx, &ent);

	while (!ent.is_none) {
		char* fullpath = (char*)malloc(MAX_PATH_SIZE);
		sprintf(fullpath, "%s/%s", path, ent.name);

		// ignore things that are not files or folders
		if (ent.type != ENTRY_FILE && ent.type != ENTRY_DIR) {
			info("ignoring node %s with unknown type\n", fullpath);
			continue;
		}

		// calculate size of node structure
		size_t nodesize;
		if (ent.type == ENTRY_DIR) {
			nodesize = sizeof(saf_node_folder_t) + (childcnt(fullpath) * sizeof(saf_offset_t));
		} else {
			nodesize = sizeof(saf_node_file_t);
		}
		
		
		size += nodesize;

		// the node is a folder
		if (ent.type == ENTRY_DIR) {
			size_t children = calculate_size(fullpath);
			size += children;
		}
		// the node is a file
		else if (ent.type == ENTRY_FILE) {
			FILE* f = fopen(fullpath, "rb");
			if (!f) {
				panic("could not open file %s\n", fullpath);
			}
			fseek(f, 0, SEEK_END);
			size += ftell(f);
			fclose(f);
		}

        dir_at(res_path, ++idx, &ent);
	}
	return size;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage: saf-make <folder-name> <archive-name> [-q]\n");
		exit(-1);
	}
	is_quiet = (argc == 4);

	char* input = argv[1];
	char* output = "archive.saf";
	if (argc == 2) {
		info("using default output filename '%s'\n", output);
	} else {
		output = argv[2];
	}
	
	size_t alloc_needed = calculate_size(input) + sizeof(saf_node_folder_t) + (childcnt(input) * sizeof(saf_offset_t));

	adata = (arch_data_t) {
		.ptr = malloc(alloc_needed),
		.curr_offset = 0,
		.alloc_size = alloc_needed
	};

	memset(adata.ptr, 0, alloc_needed);

	// create the root node
	saf_node_folder_t* root = (saf_node_folder_t*)(adata.ptr + adata.curr_offset);
	size_t nodesize = sizeof(saf_node_folder_t) + (childcnt(input) * sizeof(saf_offset_t));
	inc_offset(nodesize);
	root->hdr.magic = MAGIC_NUMBER;
	root->hdr.len = nodesize;
	root->hdr.flags = FLAG_ISFOLDER;
	nodelist_t* children = make_node(input);
	root->num_children = children->len;
	for (size_t i = 0; i < children->len; i++) {
		root->children[i] = children->offsets[i];
	}

	info("final archive size: %d bytes\n", adata.curr_offset);
	FILE* of = fopen(output, "wb");
	fwrite(adata.ptr, 1, adata.curr_offset, of);
	fclose(of);
	info("done\n");

    return 0;
}