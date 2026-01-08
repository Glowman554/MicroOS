#pragma once

// Kernel configuration
// Debug
#define DEBUG
// #define GDB_DEBUG
#define UBSAN_SUPRES_TYPE_MISSMATCH
#define UBSAN_IGNORE_SHIFT_OUT_OF_BOUNDS
#define WAIT
#define ALLOW_PANIC_CONTINUE
// #define STACK_TRACE
#define EARLY_SERIAL_DEBUG
// End Debug

// Network
#define SOCK_OFFSET 5
#define TCP_SIMULATED_DROP_PERCENT 0
#define TCP_RETRANSMIT_TIMEOUT_MS 100
#define TCP_RETRANSMIT_MAX_RETRIES 8
#define TCP_CTL_TIMEOUT_MS 300
#define TCP_CTL_MAX_RETRIES 8
// End Network

// Scheduler
#define MAX_TASKS 32
#define USER_STACK_SIZE_PAGES 16
#define KERNEL_STACK_SIZE_PAGES 16
#define MAX_CPU 4
#define MAX_ASYNC_TASKS 16
// End Scheduler

// Experimental
// #define SMP
// #define PARSE_MADT
// #define THREADS
// #define AHCI_DRIVER
// #define NETWORK_STACK
// #define TCP
// End Experimental

// Input
#define DEFAULT_LAYOUT "de"
// End Input

// Output
// #define TEXT_MODE_EMULATION
// #define STATUS_BAR
// #define RAW_FRAMEBUFFER_ACCESS
// #define FULL_SCREEN_TERMINAL
#define MAX_VTERM 4
#define SERIAL_INSERT_CARRIAGE_RETURN
// End Output

// Other
#define FD_OFFSET 5
#define FAST_MEMORY
// End Other

// Paging
#define PAGING_DATA_PAGES 512
// End Paging

// Syscalls
#define SYS_OPEN_ID 1
#define SYS_CLOSE_ID 2
#define SYS_READ_ID 3
#define SYS_WRITE_ID 4
#define SYS_FILESIZE_ID 5
#define SYS_DELETE_ID 6
#define SYS_MKDIR_ID 7
#define SYS_DIR_AT_ID 8
#define SYS_TOUCH_ID 9
#define SYS_DELETE_DIR_ID 10
#define SYS_FS_AT_ID 11
#define SYS_ASYNC_GETC_ID 12
#define SYS_EXIT_ID 13
#define SYS_MMAP_ID 14
#define SYS_SPAWN_ID 15
#define SYS_GET_PROC_INFO_ID 16
#define SYS_YIELD_ID 17
#define SYS_ENV_ID 18
#define SYS_MMMAP_ID 19
#define SYS_VMODE_ID 20
#define SYS_VPOKE_ID 21
#define SYS_VCURSOR_ID 22
#define SYS_ICMP_ID 23
#define SYS_SOCK_CONNECT_ID 26
#define SYS_SOCK_DISCONNECT_ID 27
#define SYS_SOCK_SEND_ID 28
#define SYS_SOCK_RECV_ID 29
#define SYS_SOUND_RUN_ID 30
#define SYS_TIME_ID 31
#define SYS_SET_COLOR_ID 32
#define SYS_ASYNC_GETARRW_ID 33
#define SYS_VCURSOR_GET_ID 34
#define SYS_TASK_LIST_GET_ID 35
#define SYS_KILL_ID 36
#define SYS_VPEEK_ID 37
#define SYS_RAMINFO_ID 38
#define SYS_MOUSE_INFO_ID 39
#define SYS_TRUNCATE_ID 40
#define SYS_TIME_MS_ID 41
#define SYS_THREAD_ID 42
#define SYS_SET_TERM_ID 43
#define SYS_IPV4_RESOLVE_ROUTE_ID 44
#define SYS_SOCK_SET_LOCAL_PORT_ID 45
#define SYS_MMAP_MAPPED_ID 46
// End Syscalls

// End Kernel configuration

