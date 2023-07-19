#pragma once

#include <config.h>

#include <driver/char_output_driver.h>

int printf(const char *format, ...);
int abortf(const char *format, ...);
int debugf_intrnl(const char *format, ...);

extern char_output_driver_t* debugf_driver;
extern char_output_driver_t* printf_driver;

int sprintf(char *buf, const char *fmt, ...);

void stacktrace_print(int frame_num, uint32_t eip);

int read_core_id();

#ifdef DEBUG
#define debugf(fmt, ...) debugf_intrnl("[%d] [%s:%d in %s] ", read_core_id(), __FILE__, __LINE__, __FUNCTION__); debugf_intrnl(fmt, ##__VA_ARGS__); debugf_intrnl("\n")
#else
#define debugf(fmt, ...)
#endif

#define todo() abortf("%s not implemented! (yet i hope)", __FUNCTION__)
#define note(n) debugf("note in %s (%s:%d): %s", __FUNCTION__, __FILE__, __LINE__, n)
#define here() debugf("here");

#ifdef DEBUG
#ifdef WAIT
char read_serial();
#define wait() { debugf("press any key continue executions..."); while(!read_serial()); }
#else
#define wait()
#endif
#else
#define wait()
#endif