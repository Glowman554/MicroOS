#pragma once

#include <config.h>

#include <driver/char_output_driver.h>

int printf(const char *format, ...);
int abortf(const char *format, ...);
int debugf_intrnl(const char *format, ...);

extern char_output_driver_t* debugf_driver;
extern char_output_driver_t* printf_driver;

#ifdef DEBUG
#define debugf(fmt, ...) debugf_intrnl("[%s:%d in %s] ", __FILE__, __LINE__, __FUNCTION__); debugf_intrnl(fmt, ##__VA_ARGS__); debugf_intrnl("\n")
#else
#define debugf(fmt, ...)
#endif

#define here() debugf("here");