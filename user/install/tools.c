#include <tools.h>
#include <sys/file.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void copy_dir_across_fs(char* src_fs, char* dest_fs, char* path) {
	char current_path[512] = { 0 };
	strcpy(current_path, src_fs);
	strcat(current_path, path);

	char dest_fs_new_dir[512] = { 0 };
	strcpy(dest_fs_new_dir, dest_fs);
	strcat(dest_fs_new_dir, path);

	dir_t dir;
    dir_at(current_path, 0, &dir);
	while (!dir.is_none) {
		if (dir.type == ENTRY_FILE) {
			copy_file_across_fs(src_fs, dest_fs, path, dir.name);
		} else {
			char new_path[512] = { 0 };
			strcpy(new_path, path);
			strcat(new_path, "/");
			strcat(new_path, dir.name);

			char new_dir[512] = { 0 };
			strcpy(new_dir, dest_fs);
			strcat(new_dir, new_path);
			mkdir(new_dir);

			copy_dir_across_fs(src_fs, dest_fs, new_path);
		}

        dir_at(current_path, dir.idx + 1, &dir);
    }
}

void copy_file_across_fs(char* src_fs, char* dest_fs, char* path, char* file) {
	char current_path_dest[512] = { 0 };
	strcpy(current_path_dest, dest_fs);
	strcat(current_path_dest, path);
	strcat(current_path_dest, "/");
	strcat(current_path_dest, file);
		
	char current_path_src[512] = { 0 };
	strcpy(current_path_src, src_fs);
	strcat(current_path_src, path);
	strcat(current_path_src, "/");
	strcat(current_path_src, file);

	printf("[COPY]  %s -> %s\n", current_path_src, current_path_dest);

	FILE* src = fopen(current_path_src, "r");
	if (src == NULL) {
		printf("Could not open file: %s\n", current_path_src);
		abort();
	}

	touch(current_path_dest);
	FILE* dest = fopen(current_path_dest, "w");
	if (dest == NULL) {
		printf("Could not open file: %s\n", current_path_dest);
		abort();
	}

    fsize(src, src_size);

	uint8_t* buffer = malloc(src_size);
	fread(buffer, src_size, 1, src);
	fwrite(buffer, src_size, 1, dest);

	free(buffer);
	fclose(src);
	fclose(dest);
}

void create_directory(char* dest_fs, char* path) {
	printf("[MKDIR] %s\n", path);

	char current_path[512] = { 0 };
	strcpy(current_path, dest_fs);
	strcat(current_path, path);

	mkdir(current_path);
}

void write_text_file(char* dest_fs, char* path, char* text) {
	printf("[WRITE] %s\n", path);

	char current_path[512] = { 0 };
	strcpy(current_path, dest_fs);
	strcat(current_path, path);

	touch(current_path);
	FILE* file = fopen(current_path, "w");
	if (file == NULL) {
		printf("Could not open file: %s\n", current_path);
		abort();
	}

	fwrite(text, strlen(text), 1, file);
	fclose(file);
}