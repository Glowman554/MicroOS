#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/file.h>

struct FILE* stdout;
struct FILE* stdin;
struct FILE* stderr;

FILE* fopen(const char* filename, const char* mode) {
	char filename_full[256];
	memset(filename_full, 0, 256);
	bool exists = resolve((char*) filename, filename_full);

	if (!exists && (strcmp((char*) mode, "w") == 0 || strcmp((char*) mode, "wb") == 0 || strcmp((char*) mode, "w+") == 0)) {
		touch(filename_full);
	}

	int file_mode = 0;
	if (strcmp((char*) mode, "r") == 0 || strcmp((char*) mode, "rb") == 0) {
		file_mode = FILE_OPEN_MODE_READ;
	} else if (strcmp((char*) mode, "w") == 0 || strcmp((char*) mode, "wb") == 0) {
		file_mode = FILE_OPEN_MODE_WRITE;
	} else if (strcmp((char*) mode, "w+") == 0 || strcmp((char*) mode, "r+") == 0) {
		file_mode = FILE_OPEN_MODE_READ_WRITE;
	} else {
		return NULL;
	}

	int fd = open((char*) filename_full, file_mode);
	if (fd == -1) {
		return NULL;
	}

	FILE* file = malloc(sizeof(FILE));
	file->inner_fd = fd;
	file->pos = 0;

	return file;
}

FILE* freopen(const char* filename, const char* mode, FILE* stream) {
	fclose(stream);
	return fopen(filename, mode);
}

int fclose(FILE* stream) {
	close(stream->inner_fd);
	free(stream);

	return 0;
}

int fflush(FILE *stream) {
	write(STDERR, "fflush not implemented\n", 24, 0);
	return 0;
}

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t total = size * nmemb;
	
	read(stream->inner_fd, ptr, total, stream->pos);

	stream->pos += total;
	return 1;
}

size_t fseek(FILE* stream, long offset, int whence) {
		switch (whence) {
		case SEEK_SET:
			{
				stream->pos = offset;
			}
			break;
		case SEEK_CUR:
			{
				stream->pos += offset;
			}
			break;
		case SEEK_END:
			{
				stream->pos = filesize(stream->inner_fd);
			}
			break;
	}

	return 0;
}

size_t ftell(FILE* stream) {
	return stream->pos;
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) {
	int total = size * nmemb;
	
	write(stream->inner_fd, (void*) ptr, total, stream->pos);

	stream->pos += total;
	return 1;
}

size_t fprintf(FILE* stream, const char* format, ...) {
	va_list args;
	int i;
	char buf[1024] = {0};

	va_start(args, format);
	i = vsprintf(buf, format, args);
	va_end(args);

	write(stream->inner_fd, buf, i, stream->pos);

	stream->pos += i;
	return i;
}

size_t fputs(const char* s, FILE* stream) {
	write(stream->inner_fd, (void*) s, strlen((char*) s), stream->pos);

	stream->pos += strlen((char*) s);
	return 0;
}

size_t fputc(char c, FILE* stream) {
	write(stream->inner_fd, &c, 1, stream->pos);

	stream->pos += 1;
	return 0;
}

void init_stdio() {
	stdout = malloc(sizeof(FILE));
	memset(stdout, 0, sizeof(FILE));
	stdout->inner_fd = STDOUT;

	stdin = malloc(sizeof(FILE));
	memset(stdin, 0, sizeof(FILE));
	stdin->inner_fd = STDIN;

	stderr = malloc(sizeof(FILE));
	memset(stderr, 0, sizeof(FILE));
	stderr->inner_fd = STDERR;
}

void uninit_stdio() {
	free(stdout);
	free(stdin);
	free(stderr);
}