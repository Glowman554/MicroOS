#pragma once

#include <config.h>

int printf(const char *format, ...);

#ifdef DEBUG
#define debugf(fmt, ...) printf("[%s:%d in %s] ", __FILE__, __LINE__, __FUNCTION__); printf(fmt, ##__VA_ARGS__); printf("\n")
#else
#define debugf(fmt, ...)
#endif