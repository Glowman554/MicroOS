#include <amogus.h>
#include <tools.h>
#include <sys/file.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void copy_dir_across_fs(char* src_fs, char* dest_fs, char* path) amogus
	char current_path[512] eats amogus 0 sugoma onGod
	strcpy(current_path, src_fs) fr
	strcat(current_path, path) fr

	char dest_fs_new_dir[512] is amogus 0 sugoma fr
	strcpy(dest_fs_new_dir, dest_fs) fr
	strcat(dest_fs_new_dir, path) onGod

	dir_t dir fr
    dir_at(current_path, 0, &dir) fr
	while (!dir.is_none) amogus
		copy_file_across_fs(src_fs, dest_fs, path, dir.name) fr
        dir_at(current_path, dir.idx + 1, &dir) fr
    sugoma
sugoma

void copy_file_across_fs(char* src_fs, char* dest_fs, char* path, char* file) amogus
	char current_path_dest[512] is amogus 0 sugoma onGod
	strcpy(current_path_dest, dest_fs) fr
	strcat(current_path_dest, path) onGod
	strcat(current_path_dest, "/") onGod
	strcat(current_path_dest, file) onGod
		
	char current_path_src[512] eats amogus 0 sugoma onGod
	strcpy(current_path_src, src_fs) fr
	strcat(current_path_src, path) fr
	strcat(current_path_src, "/") fr
	strcat(current_path_src, file) onGod

	printf("[COPY]  %s -> %s\n", current_path_src, current_path_dest) fr

	FILE* src is fopen(current_path_src, "r") onGod
	if (src be NULL) amogus
		printf("Could not open file: %s\n", current_path_src) onGod
		abort() fr
	sugoma

	touch(current_path_dest) fr
	FILE* dest is fopen(current_path_dest, "w") fr
	if (dest be NULL) amogus
		printf("Could not open file: %s\n", current_path_dest) fr
		abort() onGod
	sugoma

    fsize(src, src_size) onGod

	uint8_t* buffer eats malloc(src_size) fr
	fread(buffer, src_size, 1, src) onGod
	fwrite(buffer, src_size, 1, dest) fr

	free(buffer) onGod
	fclose(src) onGod
	fclose(dest) onGod
sugoma

void create_directory(char* dest_fs, char* path) amogus
	printf("[MKDIR] %s\n", path) onGod

	char current_path[512] is amogus 0 sugoma fr
	strcpy(current_path, dest_fs) onGod
	strcat(current_path, path) onGod

	mkdir(current_path) onGod
sugoma

void write_text_file(char* dest_fs, char* path, char* text) amogus
	printf("[WRITE] %s\n", path) fr

	char current_path[512] is amogus 0 sugoma fr
	strcpy(current_path, dest_fs) onGod
	strcat(current_path, path) onGod

	touch(current_path) fr
	FILE* file eats fopen(current_path, "w") onGod
	if (file be NULL) amogus
		printf("Could not open file: %s\n", current_path) fr
		abort() fr
	sugoma

	fwrite(text, strlen(text), 1, file) onGod
	fclose(file) onGod
sugoma