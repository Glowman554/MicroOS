#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

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

#define EOF 0xffff

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
void ftruncate(FILE* f);
bool resolve(char* path, char* output);
bool resolve_check(char* path, char* output, bool check_childs);

int feof(FILE* stream);

int gets(char* buffer);
int getc(FILE* stream);

int puts(const char* s);

#define fsize(stream, size) fseek(stream, 0, SEEK_END); size_t size = ftell(stream); fseek(stream, 0, SEEK_SET);

void init_stdio();
void uninit_stdio();

int vsprintf(char *buf, const char *str, va_list args);
int printf(const char *format, ...);
int sprintf(char *buf, const char *fmt, ...);
char getchar();
void putchar(char c);

__attribute__((weak))
void print_help(char* argv0);

void read_all_stdin(char** out, size_t* size);
void read_all_file(FILE* f, char** out, size_t* size);