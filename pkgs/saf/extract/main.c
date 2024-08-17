#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <buildin/ansi.h>
#include <saf.h>

// port from https://github.com/chocabloc/saf/blob/main/saf-extract.c

#define MAX_PATH_SIZE 1024

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
	char buf[512] = { 0 };
    ansi_printf("\033[32minfo: \033[0m");
    va_list args;
	va_start(args, msg);
	vsprintf(buf, msg, args);
	va_end(args);
	printf("%s", buf);
}
static void parsenode(char* path, void* base, saf_node_hdr_t* node, int level) {
	if (node->magic != MAGIC_NUMBER) {
		panic("archive is damaged or of invalid format\n");
	}

	info("processing node '%s'\n", node->name);

	char* fullpath = malloc(MAX_PATH_SIZE);
	sprintf(fullpath, "%s%s%s", path, path[strlen(path) - 1] == '/' ? "" : "/", node->name);

	if (node->flags == FLAG_ISFOLDER) {
		saf_node_folder_t* fldr = (saf_node_folder_t*)node;
		char folder[512] = { 0 };
		resolve(fullpath, folder);
		mkdir(folder);
		for (size_t i = 0; i < fldr->num_children; i++) {
			parsenode(fullpath, base, (saf_node_hdr_t*)((uint8_t*)base + fldr->children[i]), level + 1);
		}
	} else {
		saf_node_file_t* file = (saf_node_file_t*)node;
		FILE* f = fopen(fullpath, "wb");
		if (f == NULL) {
			info("failed to open file '%s' for writing\n", fullpath);
			return;
		}
		fwrite((uint8_t*)base + file->addr, 1, file->size, f);
		fclose(f);
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage: saf-extract <archive-name> <output-dir>\n");
		exit(-1);
	}

	char* input = argv[1];
	char* output = "extr";
	if (argc == 2) {
		info("using output folder '%s'\n", output);
	} else {
		output = argv[2];
	}

	FILE* f = fopen(input, "rb");
	if (!f) {
		panic("could not open file '%s'\n", input);
	}

	void* buff;
	size_t bufflen;

	fseek(f, 0, SEEK_END);
	bufflen = ftell(f);
	fseek(f, 0, SEEK_SET);

	buff = malloc(bufflen);
	fread(buff, 1, bufflen, f);

	parsenode(output, buff, (saf_node_hdr_t*)buff, 0);

    return 0;
}