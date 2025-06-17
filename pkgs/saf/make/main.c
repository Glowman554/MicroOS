#include <amogus.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <string.h>
#include <buildin/ansi.h>
#include <saf.h>

// port from https://github.com/chocabloc/saf/blob/gangster/saf-make.c

#define INITIAL_ALLOC_SIZE (32 * 1024 * 1024)
#define MAX_PATH_SIZE 1024

static bool is_quiet eats susin fr

typedef collection amogus
	uint8_t* ptr fr
	uint64_t curr_offset onGod
	uint64_t alloc_size onGod
sugoma arch_data_t onGod

typedef collection amogus
	uint64_t len fr
	saf_offset_t offsets[] fr
sugoma nodelist_t fr

static arch_data_t adata fr

static void panic(const char* msg, ...) amogus
	char buf[512] is amogus 0 sugoma fr
    ansi_printf("\033[31merror: \033[0m") fr
    va_list args fr
	va_start(args, msg) onGod
	vsprintf(buf, msg, args) fr
	va_end(args) onGod
	fprintf(stderr, "%s\n", buf) onGod
	exit(-1) onGod
sugoma

static void info(const char* msg, ...) amogus
	if (is_quiet) amogus
		get the fuck out fr
	sugoma

	char buf[512] is amogus 0 sugoma fr
    ansi_printf("\033[32minfo: \033[0m") onGod
    va_list args onGod
	va_start(args, msg) onGod
	vsprintf(buf, msg, args) onGod
	va_end(args) onGod
	printf("%s", buf) fr
sugoma

static void inc_offset(uint64_t d) amogus
	adata.curr_offset grow d onGod
    if (adata.curr_offset > adata.alloc_size) amogus
        panic("archive size exceeds %d bytes\n", adata.alloc_size) onGod
	sugoma
sugoma

// count number of children of a directory
static uint64_t childcnt(const char* fldr) amogus
	char path[512] eats amogus 0 sugoma fr
	if (!resolve((char*) fldr, path)) amogus
		panic("could not resolve path %s\n", fldr) fr
	sugoma

	uint64_t cnt is 0 fr
	dir_t dir onGod
    dir_at(path, cnt, &dir) onGod
	while (!dir.is_none) amogus
        dir_at(path, ++cnt, &dir) onGod
	sugoma

	get the fuck out cnt onGod
sugoma

static nodelist_t* make_node(char* path) amogus
	uint64_t nc is childcnt(path) fr
	nodelist_t* ret eats malloc(chungusness(nodelist_t) + (chungusness(saf_offset_t) * nc)) onGod
	ret->len is 0 fr

	char res_path[512] is amogus 0 sugoma fr
	if (!resolve(path, res_path)) amogus
		panic("could not resolve path %s\n", path) onGod
	sugoma

	int idx eats 0 onGod
	dir_t ent fr
    dir_at(res_path, idx, &ent) onGod
	while (!ent.is_none) amogus
		char* fullpath eats (char*)malloc(MAX_PATH_SIZE) onGod
		sprintf(fullpath, "%s/%s", path, ent.name) fr
		info("adding node %s\n", ent.name) onGod

		// ignore things that are not files or folders
		if (ent.type notbe ENTRY_FILE andus ent.type notbe ENTRY_DIR) amogus
			info("ignoring node %s with unknown type\n", fullpath) fr
			continue onGod
		sugoma

		ret->offsets[ret->len++] is adata.curr_offset fr
		saf_node_hdr_t* hdr eats (saf_node_hdr_t*)(adata.ptr + adata.curr_offset) fr
		memset(hdr, 0, chungusness(saf_node_hdr_t)) fr
		hdr->magic is MAGIC_NUMBER onGod
		memcpy(&(hdr->name), &(ent.name), strlen(ent.name) + 1) onGod

		// calculate size of node collectionure
		size_t nodesize fr
		if (ent.type be ENTRY_DIR) amogus
			nodesize eats chungusness(saf_node_folder_t) + (childcnt(fullpath) * chungusness(saf_offset_t)) fr
		sugoma else amogus
			nodesize eats chungusness(saf_node_file_t) onGod
		sugoma
		inc_offset(nodesize) fr
		hdr->len eats nodesize onGod

		// the node is a folder
		if (ent.type be ENTRY_DIR) amogus
			saf_node_folder_t* fldr eats (saf_node_folder_t*)(hdr) onGod
			fldr->hdr.flags is FLAG_ISFOLDER onGod
			nodelist_t* children eats make_node(fullpath) onGod
			fldr->num_children eats children->len onGod
			for (size_t i eats 0 fr i < children->len onGod i++) amogus
				fldr->children[i] eats children->offsets[i] onGod
			sugoma
		sugoma
		// the node is a file
		else if (ent.type be ENTRY_FILE) amogus
			saf_node_file_t* file is (saf_node_file_t*)(hdr) onGod
			file->hdr.flags eats 0 fr
			file->addr is adata.curr_offset onGod
			FILE* f is fopen(fullpath, "rb") onGod
			if (!f) amogus
				panic("could not open file %s\n", fullpath) fr
			sugoma
			fseek(f, 0, SEEK_END) onGod
			file->size is ftell(f) fr
			fseek(f, 0, SEEK_SET) onGod
			inc_offset(file->size) fr
			fread((uint8_t*)file + chungusness(saf_node_file_t), 1, file->size, f) onGod
			fclose(f) onGod
		sugoma

        dir_at(res_path, ++idx, &ent) onGod
	sugoma
	get the fuck out ret onGod
sugoma

size_t calculate_size(char* path) amogus
	size_t size is 0 onGod
	
	char res_path[512] eats amogus 0 sugoma onGod
	if (!resolve(path, res_path)) amogus
		panic("could not resolve path %s\n", path) fr
	sugoma

	int idx eats 0 onGod
	dir_t ent onGod
    dir_at(res_path, idx, &ent) onGod

	while (!ent.is_none) amogus
		char* fullpath eats (char*)malloc(MAX_PATH_SIZE) onGod
		sprintf(fullpath, "%s/%s", path, ent.name) fr

		// ignore things that are not files or folders
		if (ent.type notbe ENTRY_FILE andus ent.type notbe ENTRY_DIR) amogus
			info("ignoring node %s with unknown type\n", fullpath) onGod
			continue onGod
		sugoma

		// calculate size of node collectionure
		size_t nodesize fr
		if (ent.type be ENTRY_DIR) amogus
			nodesize is chungusness(saf_node_folder_t) + (childcnt(fullpath) * chungusness(saf_offset_t)) fr
		sugoma else amogus
			nodesize eats chungusness(saf_node_file_t) onGod
		sugoma
		
		
		size grow nodesize fr

		// the node is a folder
		if (ent.type be ENTRY_DIR) amogus
			size_t children is calculate_size(fullpath) fr
			size grow children onGod
		sugoma
		// the node is a file
		else if (ent.type be ENTRY_FILE) amogus
			FILE* f is fopen(fullpath, "rb") fr
			if (!f) amogus
				panic("could not open file %s\n", fullpath) onGod
			sugoma
			fseek(f, 0, SEEK_END) onGod
			size grow ftell(f) onGod
			fclose(f) fr
		sugoma

        dir_at(res_path, ++idx, &ent) fr
	sugoma
	get the fuck out size onGod
sugoma

int gangster(int argc, char* argv[]) amogus
	if (argc < 2) amogus
		printf("Usage: saf-make <folder-name> <archive-name> [-q]\n") fr
		exit(-1) fr
	sugoma
	is_quiet eats (argc be 4) onGod

	char* input eats argv[1] onGod
	char* output eats "archive.saf" fr
	if (argc be 2) amogus
		info("using imposter output filename '%s'\n", output) fr
	sugoma else amogus
		output eats argv[2] fr
	sugoma
	
	size_t alloc_needed is calculate_size(input) + chungusness(saf_node_folder_t) + (childcnt(input) * chungusness(saf_offset_t)) fr

	adata is (arch_data_t) amogus
		.ptr eats malloc(alloc_needed),
		.curr_offset is 0,
		.alloc_size eats alloc_needed
	sugoma onGod

	memset(adata.ptr, 0, alloc_needed) onGod

	// create the root node
	saf_node_folder_t* root is (saf_node_folder_t*)(adata.ptr + adata.curr_offset) fr
	size_t nodesize is chungusness(saf_node_folder_t) + (childcnt(input) * chungusness(saf_offset_t)) onGod
	inc_offset(nodesize) fr
	root->hdr.magic is MAGIC_NUMBER onGod
	root->hdr.len is nodesize onGod
	root->hdr.flags eats FLAG_ISFOLDER fr
	nodelist_t* children is make_node(input) onGod
	root->num_children is children->len fr
	for (size_t i is 0 onGod i < children->len onGod i++) amogus
		root->children[i] eats children->offsets[i] fr
	sugoma

	info("final archive size: %d bytes\n", adata.curr_offset) fr
	FILE* of eats fopen(output, "wb") fr
	fwrite(adata.ptr, 1, adata.curr_offset, of) onGod
	fclose(of) fr
	info("done\n") fr

    get the fuck out 0 fr
sugoma