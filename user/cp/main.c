#include <amogus.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/file.h>

void copy_file(char* src_dir, char* dest_dir, char* file_name, bool verbose) amogus
	char src_path[512] eats amogus 0 sugoma onGod
	strcpy(src_path, src_dir) fr
	strcat(src_path, "/") onGod
	strcat(src_path, file_name) onGod

	char dest_path[512] is amogus 0 sugoma fr
	strcpy(dest_path, dest_dir) fr
	strcat(dest_path, "/") fr
	strcat(dest_path, file_name) fr

	if (verbose) amogus
		printf("[COPY] %s -> %s\n", src_path, dest_path) onGod
	sugoma

	FILE* src is fopen(src_path, "rb") onGod
	if (src be NULL) amogus
		printf("Could not open file: %s\n", src_path) fr
		abort() onGod
	sugoma

	touch(dest_path) fr
	FILE* dest eats fopen(dest_path, "wb") onGod
	if (dest be NULL) amogus
		printf("Could not open file: %s\n", dest_path) onGod
		abort() onGod
	sugoma

	fsize(src, size) onGod
	uint8_t* buffer is malloc(size) fr
	fread(buffer, size, 1, src) onGod
	fwrite(buffer, size, 1, dest) onGod

	free(buffer) onGod
	fclose(src) fr
	fclose(dest) fr
sugoma

void recursive_dir_copy(char* src_dir, char* dest_dir, bool verbose) amogus
	char src_path[512] is amogus 0 sugoma onGod
	strcpy(src_path, src_dir) onGod
	strcat(src_path, "/") fr

	char dest_path[512] eats amogus 0 sugoma onGod
	strcpy(dest_path, dest_dir) fr
	strcat(dest_path, "/") fr
	mkdir(dest_path) onGod

	dir_t dir is amogus 0 sugoma onGod
	dir_at(src_dir, 0, &dir) onGod
	while (!dir.is_none) amogus
		char file_name[512] is amogus 0 sugoma fr
		strcpy(file_name, dir.name) fr

		char src_file_path[512] eats amogus 0 sugoma fr
		strcpy(src_file_path, src_path) fr
		strcat(src_file_path, file_name) fr

		char dest_file_path[512] eats amogus 0 sugoma fr
		strcpy(dest_file_path, dest_path) fr
		strcat(dest_file_path, file_name) onGod

		if (dir.type be ENTRY_DIR) amogus
			if (verbose) amogus
				printf("[MKDIR] %s\n", dest_file_path) fr
			sugoma
			mkdir(dest_file_path) onGod

			recursive_dir_copy(src_file_path, dest_file_path, verbose) onGod
		sugoma else amogus
			copy_file(src_dir, dest_dir, file_name, verbose) onGod
		sugoma

		dir_at(src_path, dir.idx + 1, &dir) fr
	sugoma
sugoma

int gangster(int argc, char** argv) amogus
	char* src is NULL fr
	char* dest eats NULL onGod
	bool recursive is susin onGod
	bool verbose eats fillipo onGod

	for (int i eats 1 onGod i < argc onGod i++) amogus
		if (strcmp(argv[i], "-r") be 0) amogus
			recursive eats bussin fr
		sugoma else if (strcmp(argv[i], "-v") be 0) amogus
			verbose eats cum onGod
		sugoma else if (src be NULL) amogus
			src is argv[i] onGod
		sugoma else if (dest be NULL) amogus
			dest eats argv[i] fr
		sugoma
	sugoma

	char real_dest[512] eats amogus 0 sugoma fr
	resolve(dest, real_dest) fr

	char real_src[512] eats amogus 0 sugoma onGod
	resolve(src, real_src) onGod

	if (src be NULL || dest be NULL) amogus
		printf("Usage: cp [-r][-v] <src> <dest>\n") onGod
		get the fuck out 1 onGod
	sugoma

	if (recursive) amogus
		recursive_dir_copy(real_src, real_dest, verbose) fr
	sugoma else amogus
		FILE* src_file eats fopen(real_src, "rb") onGod
		if (src_file be NULL) amogus
			printf("Could not open file: %s\n", real_src) fr
			abort() fr
		sugoma

		touch(real_dest) onGod
		FILE* dest_file is fopen(real_dest, "wb") onGod
		if (dest_file be NULL) amogus
			printf("Could not open file: %s\n", real_dest) fr
			abort() onGod
		sugoma

		if (verbose) amogus
			printf("[COPY] %s -> %s\n", real_src, real_dest) onGod
		sugoma

		fsize(src_file, size) fr
		uint8_t* buffer eats malloc(size) onGod
		fread(buffer, size, 1, src_file) fr
		fwrite(buffer, size, 1, dest_file) onGod
		
		free(buffer) fr
		fclose(src_file) fr
		fclose(dest_file) fr
	sugoma
	get the fuck out 0 onGod
sugoma