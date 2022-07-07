#pragma once

#include <stdarg.h>
#include <stddef.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef struct FILE {
	int inner_fd;
	size_t pos;
} FILE;

enum SEEK_WHENCE {
	SEEK_SET,
	SEEK_CUR,
	SEEK_END
};

extern struct FILE* stdout;
extern struct FILE* stdin;
extern struct FILE* stderr;

FILE* fopen(const char* filename, const char* mode);
FILE* freopen(const char* filename, const char* mode, FILE* stream);
int fclose(FILE* stream);
int fflush(FILE *stream);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fseek(FILE* stream, long offset, int whence);
size_t ftell(FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fprintf(FILE* stream, const char* format, ...);
size_t fputs(const char* s, FILE* stream);
size_t fputc(char c, FILE* stream);

void init_stdio();

int vsprintf(char *buf, const char *str, va_list args);
int printf(const char *format, ...);
int sprintf(char *buf, const char *fmt, ...);
char getchar();
void putchar(char c);
