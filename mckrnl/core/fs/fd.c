#include <fs/fd.h>

#include <config.h>
#include <memory/heap.h>
#include <stddef.h>

file_t** fd_table;
int num_fd = 0;

int file_to_fd(file_t* file) {
	for (int i = 0; i < num_fd; i++) {
		if (fd_table[i] == NULL) {
			fd_table[i] = file;
			return i + FD_OFFSET;
		}
	}

	fd_table = krealloc(fd_table, sizeof(file_t*) * (num_fd + 1));
	fd_table[num_fd] = file;
	num_fd++;

	return num_fd - 1 + FD_OFFSET;
};

file_t* fd_to_file(int fd) {
	return fd_table[fd - FD_OFFSET];
}

void fd_free(int fd) {
	fd_table[fd - FD_OFFSET] = NULL;
}