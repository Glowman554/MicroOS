#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../src/tools/nextfs.c"
#include <buildin/disk_raw.h>
#include <assert.h>

int disk = 1;
struct nextfs fs;

void r(uint8_t* b, uint32_t start_sector, int count, void* data) {
    assert(count % 512 == 0);
    count /= 512;
    read_sector_raw(disk, start_sector, count, b);
}

void w(uint8_t* b, uint32_t start_sector, int count, void* data) {
    assert(count % 512 == 0);
    count /= 512;
    write_sector_raw(disk, start_sector, count, b);
}


void insert(char* file, char* internal_file) {
	FILE* data = fopen(file, "rb");
	
    if (data == NULL) {
		printf("Error while opening the file.\n");
		return;
	}
	
    fseek(data, 0, SEEK_END);
	int sz = ftell(data);
	// printf("File is %d bytes big!\n", sz);
	fseek(data, 0, SEEK_SET);
    void* buffer = malloc(sz);
	fread(buffer, 1, sz, data);
	fclose(data);
	new_file_nextfs(&fs, NULL, internal_file, buffer, sz);
    free(buffer);
}

void write_bootloader(char* file) {
	FILE* loader = fopen(file, "rb");

	if (loader == NULL) {
		printf("Error while opening the file.\n");
		return;
	}
	
	fseek(loader, 0, SEEK_END);
	int sz = ftell(loader);
	fseek(loader, 0, SEEK_SET);	
    void* buffer = malloc(sz);
	fread(buffer, 1, sz, loader);
    fclose(loader);

    int adjusted_size = sz + 512 - (sz % 512);
    fs.writer(buffer, 0, adjusted_size, NULL);

    free(buffer);
}

int main(int argc, char* argv[]) {

	if(argc < 2) {
		printf("Use %s <bootloader> <file;name> ...\n", argv[0]);
		return -1;
	}

	fs.reader = r;
	fs.writer = w;
	fs.swriter = (screen_write) printf;

	char* diskStr = getenv("DISK");
	if (diskStr != NULL) {
		disk = strtol(diskStr, NULL, 10);
		printf("Using disk %d\n", disk);
	} else {
		printf("Please set the DISK environment variable.\n");
		abort();
	}

	init_nextfs(&fs, NULL);
	format_nextfs(&fs, "test");

	write_bootloader(argv[1]);

	for (int i = 0; i < argc - 2; i++) {
		char* file = strtok(argv[i + 2], ";");
		char* file_internal = strtok(NULL, ";");

		// printf("%s -> %s\n", file, file_internal);
		insert(file, file_internal);
	}
	

	print_nextfs(&fs);
	uninit_nextfs(&fs, NULL);

	return 0;
}