#pragma once

#include <stddef.h>

char* strcpy(char* dest, const char* src);
size_t strlen(const char* s);
size_t strnlen(const char* s, size_t maxlen);
char* strcat(char* dest, const char* src);
int strcmp(const char* str1, const char* str2);
char* strchr(const char* str, int c);

int sprintf(char* str, const char* format, ...);

void* memcpy(void* dest, const void* src, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
void* memset(void* s, int c, size_t n);
void* memmove(void* dest, const void* src, size_t n);

char* strtok(char* str, const char* delim);

int strncmp(const char* str1, const char* str2, size_t n);

char* strndup(const char* str, size_t n);

char* strstr(const char* haystack, const char* needle);

char* strdup(const char* str);