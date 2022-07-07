#pragma once

#include <renderer/text_console.h>

#define PRINTF_OUTPUT_FUNCTION text_console_puts
#define DEBUG
#define UBSAN_SUPRES_TYPE_MISSMATCH

#define MAX_TASKS 32
#define MAX_DRIVERS 32
#define MAX_VFS_MOUNTS 32
#define MAX_SYSCALLS 32
#define MAX_FD 32

#define FD_OFFSET 5

#define USER_STACK_SIZE_PAGES 4
#define KERNEL_STACK_SIZE_PAGES 4

#define SYS_OPEN_ID 0x1
#define SYS_CLOSE_ID 0x2
#define SYS_READ_ID 0x3
#define SYS_WRITE_ID 0x4
#define SYS_FILESIZE_ID 0x5
#define SYS_ASYNC_GETC_ID 0x6