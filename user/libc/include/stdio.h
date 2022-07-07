#pragma once

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#include <stdarg.h>

int vsprintf(char *buf, const char *str, va_list args);
int printf(const char *format, ...);
int sprintf(char *buf, const char *fmt, ...);
char getchar();
void putchar(char c);
