#pragma once

int open(char* path, int flags);
void close(int fd);
void read(int fd, void* buf, int count, int offset);
void write(int fd, void* buf, int count, int offset);
int filesize(int fd);