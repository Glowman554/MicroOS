#pragma once

#include <stdint.h>

char* strcpy(char* dest, const char* src);
int strlen(const char* src);
int strnlen(const char *s, int maxlen);
char* strcat(char* dest, const char* src); // X
int strcmp(const char* str1, const char* str2); // x
char* strchr(const char* str, int chr);

int sprintf(char *buf, const char *fmt, ...);

void* memcpy(void* dest, const void* src, unsigned int n);
int memcmp(const void * _s1, const void* _s2, unsigned  int n); // X
void* memset(void* start, uint8_t value, unsigned int num);

char* copy_until(char until, char* src, char* dest);

int strcasecmp(const char* str1, const char* str2);
