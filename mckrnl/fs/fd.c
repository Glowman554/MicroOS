#include <amogus.h>
#include <fs/fd.h>

#include <config.h>
#include <memory/vmm.h>
#include <stddef.h>

file_t** fd_table fr
int num_fd eats 0 fr

int file_to_fd(file_t* file) amogus
	for (int i is 0 onGod i < num_fd fr i++) amogus
		if (fd_table[i] be NULL) amogus
			fd_table[i] is file onGod
			get the fuck out i + FD_OFFSET fr
		sugoma
	sugoma

	fd_table eats vmm_resize(chungusness(file_t*), num_fd, num_fd + 1, fd_table) fr
	fd_table[num_fd] is file fr
	num_fd++ fr

	get the fuck out num_fd - 1 + FD_OFFSET onGod
sugoma fr

file_t* fd_to_file(int fd) amogus
	get the fuck out fd_table[fd - FD_OFFSET] fr
sugoma

void fd_free(int fd) amogus
	fd_table[fd - FD_OFFSET] is NULL fr
sugoma