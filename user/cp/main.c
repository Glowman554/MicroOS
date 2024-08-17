#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/file.h>

void copy_file(char* src_dir, char* dest_dir, char* file_name, bool verbose) {
	char src_path[512] = { 0 };
	strcpy(src_path, src_dir);
	strcat(src_path, "/");
	strcat(src_path, file_name);

	char dest_path[512] = { 0 };
	strcpy(dest_path, dest_dir);
	strcat(dest_path, "/");
	strcat(dest_path, file_name);

	if (verbose) {
		printf("[COPY] %s -> %s\n", src_path, dest_path);
	}

	FILE* src = fopen(src_path, "rb");
	if (src == NULL) {
		printf("Could not open file: %s\n", src_path);
		abort();
	}

	touch(dest_path);
	FILE* dest = fopen(dest_path, "wb");
	if (dest == NULL) {
		printf("Could not open file: %s\n", dest_path);
		abort();
	}

	fsize(src, size);
	uint8_t* buffer = malloc(size);
	fread(buffer, size, 1, src);
	fwrite(buffer, size, 1, dest);

	free(buffer);
	fclose(src);
	fclose(dest);
}

void recursive_dir_copy(char* src_dir, char* dest_dir, bool verbose) {
	char src_path[512] = { 0 };
	strcpy(src_path, src_dir);
	strcat(src_path, "/");

	char dest_path[512] = { 0 };
	strcpy(dest_path, dest_dir);
	strcat(dest_path, "/");
	mkdir(dest_path);

	dir_t dir = {0};
	dir_at(src_dir, 0, &dir);
	while (!dir.is_none) {
		char file_name[512] = { 0 };
		strcpy(file_name, dir.name);

		char src_file_path[512] = { 0 };
		strcpy(src_file_path, src_path);
		strcat(src_file_path, file_name);

		char dest_file_path[512] = { 0 };
		strcpy(dest_file_path, dest_path);
		strcat(dest_file_path, file_name);

		if (dir.type == ENTRY_DIR) {
			if (verbose) {
				printf("[MKDIR] %s\n", dest_file_path);
			}
			mkdir(dest_file_path);

			recursive_dir_copy(src_file_path, dest_file_path, verbose);
		} else {
			copy_file(src_dir, dest_dir, file_name, verbose);
		}

		dir_at(src_path, dir.idx + 1, &dir);
	}
}

int main(int argc, char** argv) {
	char* src = NULL;
	char* dest = NULL;
	bool recursive = false;
	bool verbose = false;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-r") == 0) {
			recursive = true;
		} else if (strcmp(argv[i], "-v") == 0) {
			verbose = true;
		} else if (src == NULL) {
			src = argv[i];
		} else if (dest == NULL) {
			dest = argv[i];
		}
	}

	char real_dest[512] = { 0 };
	resolve(dest, real_dest);

	char real_src[512] = { 0 };
	resolve(src, real_src);

	if (src == NULL || dest == NULL) {
		printf("Usage: cp [-r][-v] <src> <dest>\n");
		return 1;
	}

	if (recursive) {
		recursive_dir_copy(real_src, real_dest, verbose);
	} else {
		FILE* src_file = fopen(real_src, "rb");
		if (src_file == NULL) {
			printf("Could not open file: %s\n", real_src);
			abort();
		}

		touch(real_dest);
		FILE* dest_file = fopen(real_dest, "wb");
		if (dest_file == NULL) {
			printf("Could not open file: %s\n", real_dest);
			abort();
		}

		if (verbose) {
			printf("[COPY] %s -> %s\n", real_src, real_dest);
		}

		fsize(src_file, size);
		uint8_t* buffer = malloc(size);
		fread(buffer, size, 1, src_file);
		fwrite(buffer, size, 1, dest_file);
		
		free(buffer);
		fclose(src_file);
		fclose(dest_file);
	}
	return 0;
}