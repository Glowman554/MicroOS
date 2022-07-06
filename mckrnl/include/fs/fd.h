#pragma once

#include <config.h>
#include <fs/vfs.h>

int file_to_fd(file_t* file);
file_t* fd_to_file(int fd);
void fd_free(int fd);