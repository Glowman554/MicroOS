#pragma once

#define DEBUG
#define UBSAN_SUPRES_TYPE_MISSMATCH
#define UBSAN_IGNORE_SHIFT_OUT_OF_BOUNDS

#define MAX_TASKS 32

#define FD_OFFSET 5
#define SOCK_OFFSET 5

#define USER_STACK_SIZE_PAGES 4
#define KERNEL_STACK_SIZE_PAGES 4

// #define SMP
#define SMP_TRAMPOLINE_PAGE 8
#define SMP_TRAMPOLINE_ADDR (SMP_TRAMPOLINE_PAGE * 0x1000)

#define WAIT // enable the wait macro

#define ALLOW_PANIC_CONTINUE

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

#define SYS_VMODE_ID 0x14
#define SYS_VPOKE_ID 0x15
#define SYS_VCURSOR_ID 0x16

#define SYS_ICMP_ID 0x17
#define SYS_DNS_A_ID 0x18

#define SYS_SOCK_CONNECT_ID 0x1a
#define SYS_SOCK_DISCONNECT_ID 0x1b
#define SYS_SOCK_SEND_ID 0x1c
#define SYS_SOCK_RECV_ID 0x1d

#define SYS_SOUND_RUN_ID 0x1e

#define SYS_TIME_ID 0x1f

#define SYS_SET_COLOR_ID 0x20

#define SYS_ASYNC_GETARRW_ID 0x21

#define SYS_VCURSOR_GET_ID 0x22

#define SYS_TASK_LIST_GET_ID 0x23

#define SYS_KILL_ID 0x24

#define HOSTNAME "MicroOS"