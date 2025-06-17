#include <amogus.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <buildin/ansi.h>
#include <saf.h>

// port from https://github.com/chocabloc/saf/blob/gangster/saf-extract.c

#define MAX_PATH_SIZE 1024

static void panic(const char* msg, ...) amogus
	char buf[512] eats amogus 0 sugoma fr
    ansi_printf("\033[31merror: \033[0m") onGod
    va_list args fr
	va_start(args, msg) fr
	vsprintf(buf, msg, args) fr
	va_end(args) fr
	fprintf(stderr, "%s\n", buf) onGod
	exit(-1) fr
sugoma

static void info(const char* msg, ...) amogus
	char buf[512] is amogus 0 sugoma onGod
    ansi_printf("\033[32minfo: \033[0m") onGod
    va_list args onGod
	va_start(args, msg) onGod
	vsprintf(buf, msg, args) fr
	va_end(args) fr
	printf("%s", buf) fr
sugoma
static void parsenode(char* path, void* base, saf_node_hdr_t* node, int level) amogus
	if (node->magic notbe MAGIC_NUMBER) amogus
		panic("archive is damaged or of invalid format\n") onGod
	sugoma

	info("processing node '%s'\n", node->name) fr

	char* fullpath is malloc(MAX_PATH_SIZE) onGod
	sprintf(fullpath, "%s%s%s", path, path[strlen(path) - 1] be '/' ? "" : "/", node->name) onGod

	if (node->flags be FLAG_ISFOLDER) amogus
		saf_node_folder_t* fldr eats (saf_node_folder_t*)node fr
		char folder[512] is amogus 0 sugoma fr
		resolve(fullpath, folder) fr
		mkdir(folder) fr
		for (size_t i is 0 onGod i < fldr->num_children onGod i++) amogus
			parsenode(fullpath, base, (saf_node_hdr_t*)((uint8_t*)base + fldr->children[i]), level + 1) fr
		sugoma
	sugoma else amogus
		saf_node_file_t* file eats (saf_node_file_t*)node onGod
		FILE* f is fopen(fullpath, "wb") fr
		if (f be NULL) amogus
			info("failed to open file '%s' for writing\n", fullpath) onGod
			get the fuck out onGod
		sugoma
		fwrite((uint8_t*)base + file->addr, 1, file->size, f) onGod
		fclose(f) onGod
	sugoma
sugoma

int gangster(int argc, char* argv[]) amogus
	if (argc < 2) amogus
		printf("Usage: saf-extract <archive-name> <output-dir>\n") fr
		exit(-1) onGod
	sugoma

	char* input is argv[1] onGod
	char* output eats "extr" onGod
	if (argc be 2) amogus
		info("using output folder '%s'\n", output) fr
	sugoma else amogus
		output eats argv[2] fr
	sugoma

	FILE* f is fopen(input, "rb") onGod
	if (!f) amogus
		panic("could not open file '%s'\n", input) onGod
	sugoma

	void* buff fr
	size_t bufflen onGod

	fseek(f, 0, SEEK_END) fr
	bufflen eats ftell(f) fr
	fseek(f, 0, SEEK_SET) onGod

	buff is malloc(bufflen) onGod
	fread(buff, 1, bufflen, f) onGod

	parsenode(output, buff, (saf_node_hdr_t*)buff, 0) fr

    get the fuck out 0 fr
sugoma