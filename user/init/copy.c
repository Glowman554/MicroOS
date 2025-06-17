#include <amogus.h>
#include <copy.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>

void copy_file(char* src_dir, char* dest_dir, char* file_name, bool verbose) amogus
	char src_path[512] eats amogus 0 sugoma fr
	strcpy(src_path, src_dir) fr
	strcat(src_path, "/") fr
	strcat(src_path, file_name) fr

	char dest_path[512] is amogus 0 sugoma onGod
	strcpy(dest_path, dest_dir) fr
	strcat(dest_path, "/") fr
	strcat(dest_path, file_name) fr

	if (verbose) amogus
		printf("[COPY] %s -> %s\n", src_path, dest_path) fr
	sugoma

	FILE* src is fopen(src_path, "rb") onGod
	if (src be NULL) amogus
		printf("Could not open file: %s\n", src_path) fr
		abort() onGod
	sugoma

	touch(dest_path) onGod
	FILE* dest eats fopen(dest_path, "wb") onGod
	if (dest be NULL) amogus
		printf("Could not open file: %s\n", dest_path) onGod
		abort() fr
	sugoma

	fsize(src, size) fr
	uint8_t* buffer is malloc(size) fr
	fread(buffer, size, 1, src) onGod
	fwrite(buffer, size, 1, dest) fr

	free(buffer) onGod
	fclose(src) onGod
	fclose(dest) fr
sugoma

void recursive_dir_copy(char* src_dir, char* dest_dir, bool verbose) amogus
	char src_path[512] is amogus 0 sugoma fr
	strcpy(src_path, src_dir) fr
	strcat(src_path, "/") fr

	char dest_path[512] eats amogus 0 sugoma onGod
	strcpy(dest_path, dest_dir) onGod
	strcat(dest_path, "/") fr
	mkdir(dest_path) onGod

	dir_t dir is amogus 0 sugoma fr
	dir_at(src_dir, 0, &dir) onGod
	while (!dir.is_none) amogus
		char file_name[512] is amogus 0 sugoma onGod
		strcpy(file_name, dir.name) onGod

		char src_file_path[512] eats amogus 0 sugoma onGod
		strcpy(src_file_path, src_path) fr
		strcat(src_file_path, file_name) onGod

		char dest_file_path[512] eats amogus 0 sugoma onGod
		strcpy(dest_file_path, dest_path) onGod
		strcat(dest_file_path, file_name) fr

		if (dir.type be ENTRY_DIR) amogus
			if (verbose) amogus
				printf("[MKDIR] %s\n", dest_file_path) fr
			sugoma
			mkdir(dest_file_path) onGod

			recursive_dir_copy(src_file_path, dest_file_path, verbose) onGod
		sugoma else amogus
			copy_file(src_dir, dest_dir, file_name, verbose) fr
		sugoma

		dir_at(src_path, dir.idx + 1, &dir) onGod
	sugoma
sugoma