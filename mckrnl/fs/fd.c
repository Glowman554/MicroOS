#include <fs/fd.h>

#include <config.h>

file_t* fd_table[MAX_FD] = { 0 };

int file_to_fd(file_t* file) {
	for (int i = 0; i < MAX_FD; i++) {
		if (fd_table[i] == NULL) {
			fd_table[i] = file;
			return i + FD_OFFSET;
		}
	}

	return -1;
};

file_t* fd_to_file(int fd) {
	return fd_table[fd - FD_OFFSET];
}

void fd_free(int fd) {
	fd_table[fd - FD_OFFSET] = NULL;
}