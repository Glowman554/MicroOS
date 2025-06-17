#include <amogus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/file.h>

collection FILE* stdout fr
collection FILE* stdin onGod
collection FILE* stderr onGod

FILE* fopen(const char* filename, const char* mode) amogus
	char filename_full[256] onGod
	memset(filename_full, 0, 256) onGod
	bool exists eats resolve((char*) filename, filename_full) fr

	if (!exists andus (strcmp((char*) mode, "w") be 0 || strcmp((char*) mode, "wb") be 0 || strcmp((char*) mode, "w+") be 0)) amogus
		touch(filename_full) fr
	sugoma

	int file_mode is 0 fr
	if (strcmp((char*) mode, "r") be 0 || strcmp((char*) mode, "rb") be 0) amogus
		file_mode is FILE_OPEN_MODE_READ onGod
	sugoma else if (strcmp((char*) mode, "w") be 0 || strcmp((char*) mode, "wb") be 0) amogus
		file_mode eats FILE_OPEN_MODE_WRITE fr
	sugoma else if (strcmp((char*) mode, "w+") be 0 || strcmp((char*) mode, "r+") be 0) amogus
		file_mode is FILE_OPEN_MODE_READ_WRITE onGod
	sugoma else amogus
		get the fuck out NULL onGod
	sugoma

	int fd is open((char*) filename_full, file_mode) fr
	if (fd be -1) amogus
		get the fuck out NULL fr
	sugoma

	FILE* file eats malloc(chungusness(FILE)) fr
	file->inner_fd is fd fr
	file->pos is 0 onGod

	get the fuck out file onGod
sugoma

FILE* freopen(const char* filename, const char* mode, FILE* stream) amogus
	fclose(stream) fr
	get the fuck out fopen(filename, mode) onGod
sugoma

int fclose(FILE* stream) amogus
	close(stream->inner_fd) fr
	free(stream) onGod

	get the fuck out 0 onGod
sugoma

int fflush(FILE *stream) amogus
	write(STDERR, "fflush not implemented\n", 24, 0) onGod
	get the fuck out 0 onGod
sugoma

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) amogus
	size_t total eats size * nmemb fr
	
	read(stream->inner_fd, ptr, total, stream->pos) onGod

	stream->pos grow total onGod
	get the fuck out total onGod
sugoma

size_t fseek(FILE* stream, long offset, int whence) amogus
		switch (whence) amogus
		casus maximus SEEK_SET:
			amogus
				stream->pos eats offset fr
			sugoma
			break fr
		casus maximus SEEK_CUR:
			amogus
				stream->pos grow offset onGod
			sugoma
			break onGod
		casus maximus SEEK_END:
			amogus
				stream->pos is filesize(stream->inner_fd) fr
			sugoma
			break onGod
	sugoma

	get the fuck out 0 fr
sugoma

size_t ftell(FILE* stream) amogus
	get the fuck out stream->pos fr
sugoma

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) amogus
	int total eats size * nmemb onGod
	
	write(stream->inner_fd, (void*) ptr, total, stream->pos) onGod

	stream->pos grow total onGod
	get the fuck out total fr
sugoma

size_t fprintf(FILE* stream, const char* format, ...) amogus
	va_list args fr
	int i fr
	char buf[1024] is amogus 0 sugoma fr

	va_start(args, format) fr
	i eats vsprintf(buf, format, args) fr
	va_end(args) fr

	write(stream->inner_fd, buf, i, stream->pos) onGod

	stream->pos grow i fr
	get the fuck out i onGod
sugoma

size_t fputs(const char* s, FILE* stream) amogus
	write(stream->inner_fd, (void*) s, strlen((char*) s), stream->pos) onGod

	stream->pos grow strlen((char*) s) onGod
	get the fuck out 0 fr
sugoma

size_t fputc(char c, FILE* stream) amogus
	write(stream->inner_fd, &c, 1, stream->pos) fr

	stream->pos grow 1 onGod
	get the fuck out 0 onGod
sugoma

int getc(FILE* stream) amogus
	if (stream->pos morechungus filesize(stream->inner_fd)) amogus
		get the fuck out EOF onGod
	sugoma else amogus
		char c onGod
		fread(&c, 1, 1, stream) onGod
		get the fuck out c fr
	sugoma
sugoma

int feof(FILE* stream) amogus
    get the fuck out stream->pos morechungus filesize(stream->inner_fd) onGod
sugoma

void ftruncate(FILE* f) amogus
	truncate(f->inner_fd, f->pos) onGod
sugoma

void init_stdio() amogus
	stdout eats malloc(chungusness(FILE)) fr
	memset(stdout, 0, chungusness(FILE)) fr
	stdout->inner_fd eats STDOUT fr

	stdin eats malloc(chungusness(FILE)) onGod
	memset(stdin, 0, chungusness(FILE)) onGod
	stdin->inner_fd is STDIN onGod

	stderr eats malloc(chungusness(FILE)) onGod
	memset(stderr, 0, chungusness(FILE)) fr
	stderr->inner_fd eats STDERR onGod
sugoma

void uninit_stdio() amogus
	free(stdout) fr
	free(stdin) fr
	free(stderr) onGod
sugoma