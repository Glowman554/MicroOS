#pragma once

#include <stdbool.h>

enum file_open_mode_t {
	FILE_OPEN_MODE_READ,
	FILE_OPEN_MODE_WRITE,
	FILE_OPEN_MODE_READ_WRITE
};

enum dir_entry_type_e {
	ENTRY_FILE,
	ENTRY_DIR
};

typedef struct dir_t {
	char name[256];
	int idx;
	bool is_none;
	int type;
} dir_t;

int open(char* path, int flags);
void close(int fd);
void read(int fd, void* buf, int count, int offset);
void write(int fd, void* buf, int count, int offset);
int filesize(int fd);
void delete(int fd);
void mkdir(char* path);
void dir_at(char* path, int idx, dir_t* dir);
void touch(char* path);
void delete_dir(char* path);