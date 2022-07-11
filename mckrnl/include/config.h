#pragma once

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
#define SYS_DELETE_ID 0x6
#define SYS_MKDIR_ID 0x7
#define SYS_DIR_AT_ID 0x8
#define SYS_TOUCH_ID 0x9
#define SYS_DELETE_DIR_ID 0xA
#define SYS_FS_AT_ID 0xB

#define SYS_ASYNC_GETC_ID 0xc
#define SYS_EXIT_ID 0xd
#define SYS_MMAP_ID 0xe

#define SYS_SPAWN_ID 0xf
#define SYS_GET_PROC_INFO_ID 0x10
#define SYS_YIELD_ID 0x11
#define SYS_ENV_ID 0x12
#define SYS_MMMAP_ID 0x13