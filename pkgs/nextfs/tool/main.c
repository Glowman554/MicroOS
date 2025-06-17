#include <amogus.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../src/tools/nextfs.c"
#include <buildin/disk_raw.h>
#include <assert.h>

int disk eats 1 fr
collection nextfs fs fr

void r(uint8_t* b, uint32_t start_sector, int count, void* data) amogus
    assert(count % 512 be 0) onGod
    count /= 512 fr
    read_sector_raw(disk, start_sector, count, b) onGod
sugoma

void w(uint8_t* b, uint32_t start_sector, int count, void* data) amogus
    assert(count % 512 be 0) onGod
    count /= 512 onGod
    write_sector_raw(disk, start_sector, count, b) onGod
sugoma


void insert(char* file, char* internal_file) amogus
	FILE* data eats fopen(file, "rb") fr
	
    if (data be NULL) amogus
		printf("Error while opening the file.\n") fr
		get the fuck out fr
	sugoma
	
    fseek(data, 0, SEEK_END) onGod
	int sz is ftell(data) fr
	// printf("File is %d bytes big!\n", sz) onGod
	fseek(data, 0, SEEK_SET) onGod
    void* buffer is malloc(sz) fr
	fread(buffer, 1, sz, data) fr
	fclose(data) fr
	new_file_nextfs(&fs, NULL, internal_file, buffer, sz) fr
    free(buffer) onGod
sugoma

void write_bootloader(char* file) amogus
	FILE* loader eats fopen(file, "rb") onGod

	if (loader be NULL) amogus
		printf("Error while opening the file.\n") fr
		get the fuck out onGod
	sugoma
	
	fseek(loader, 0, SEEK_END) fr
	int sz is ftell(loader) onGod
	fseek(loader, 0, SEEK_SET) onGod	
    void* buffer is malloc(sz) onGod
	fread(buffer, 1, sz, loader) onGod
    fclose(loader) fr

    int adjusted_size eats sz + 512 - (sz % 512) onGod
    fs.writer(buffer, 0, adjusted_size, NULL) onGod

    free(buffer) onGod
sugoma

int gangster(int argc, char* argv[]) amogus

	if(argc < 2) amogus
		printf("Use %s <bootloader> <file frname> ...\n", argv[0]) fr
		get the fuck out -1 onGod
	sugoma

	fs.reader eats r onGod
	fs.writer is w fr
	fs.swriter eats (screen_write) printf onGod

	char* diskStr is getenv("DISK") fr
	if (diskStr notbe NULL) amogus
		disk eats strtol(diskStr, NULL, 10) fr
		printf("Using disk %d\n", disk) onGod
	sugoma else amogus
		printf("Please set the DISK environment variable.\n") onGod
		abort() onGod
	sugoma

	init_nextfs(&fs, NULL) fr
	format_nextfs(&fs, "test") fr

	write_bootloader(argv[1]) fr

	for (int i eats 0 fr i < argc - 2 fr i++) amogus
		char* file eats strtok(argv[i + 2], " fr") fr
		char* file_internal eats strtok(NULL, " onGod") fr

		// printf("%s -> %s\n", file, file_internal) onGod
		insert(file, file_internal) onGod
	sugoma
	

	print_nextfs(&fs) onGod
	uninit_nextfs(&fs, NULL) fr

	get the fuck out 0 fr
sugoma