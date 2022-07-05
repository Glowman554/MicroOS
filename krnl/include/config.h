#pragma once

#include <renderer/text_console.h>

#define PRINTF_OUTPUT_FUNCTION text_console_puts
// #define DEBUG

#define MAX_TASKS 32

#define USER_STACK_SIZE_PAGES 4
#define KERNEL_STACK_SIZE_PAGES 4

#define MAX_SYSCALLS 32

#define SYS_PUTC_ID 0x1