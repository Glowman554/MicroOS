# Syscalls

| Syscall | eax | ebx | ecx | edx | esi | edi |
|---------|-----|-----|-----|-----|-----|-----|
| SYS_OPEN | 0x1 | path | flags | fd (return)  | - | - |
| SYS_CLOSE | 0x2 | fd | - | - | - | - |
| SYS_READ | 0x3 | fd | buf | count | offset | - |
| SYS_WRITE | 0x4 | fd | buf | count | offset | - |
| SYS_FILESIZE | 0x5 | fd | size (return) | - | - | - |
| SYS_DELETE | 0x6 | fd | - | - | - | - |
| SYS_MKDIR | 0x7 | path | - | - | - | - |
| SYS_DIR_AT | 0x8 | path | idx | &dir_t | - | - |
| SYS_TOUCH | 0x9 | path | - | - | - | - |
| SYS_DELETE_DIR | 0xa | path | - | - | - | - |
| SYS_FS_AT | 0xb | path | idx | &fs_t | - | - |
| SYS_ASYNC_GETC | 0xc | c (return) | - | - | - | - |
| SYS_EXIT | 0xd | status | - | - | - | - |
| SYS_MMAP | 0xe | addr | - | - | - | - |
| SYS_SPAWN | 0xf | path | argv | envp | pid (return) | - |
| SYS_GET_PROC_INFO | 0x10 | pid | runningn (return) | - | - | - |
| SYS_YIELD | 0x11 | - | - | - | - | - |
| SYS_ENV | 0x12 | mode | &data (return?) | - | - | - |
| SYS_MMMAP | 0x13 | ptr | ptr_remote | pid | - | - |
| SYS_VMODE | 0x14 | mode (return) | - | - | - | - |
| SYS_VPOKE | 0x15 | offset | &data | range | - | - |
| SYS_VCURSOR | 0x16 | x | y | - | - | - |
| SYS_ICMP | 0x17 | nic | ip | result (return) | - | - |
| SYS_DNS_A | 0x18 | nic | domain | result (return) | - | - |
| SYS_SOCK_CONNECT | 0x1a | nic | ip | port | socket_type / socket_id (return) | - |
| SYS_SOCK_DISCONNECT | 0x1b | socket_id | - | - | - | - |
| SYS_SOCK_SEND | 0x1c | socket_id | &data | size | - | - |
| SYS_SOCK_RECV | 0x1d | socket_id | &data | size | bytes (return) | - |
| SYS_SOUND_RUN | 0x1e | &coro | &context | - | - | - |
| SYS_TIME | 0x1f | time (return) | - | - | - | - |
| SYS_SET_COLOR | 0x20 | color | background | - | - | - |
| SYS_ASYNC_GETARRW | 0x21 | a (return) | - | - | - | - |
| SYS_VCURSOR_GET | 0x22 | &x | &y | - | - | - |
| SYS_TASK_LIST_GET | 0x23 | &out | max | actual (return) | - | - |
| SYS_KILL | 0x24 | pid | - | - | - | - |
| SYS_VPEEK | 0x25 | offset | &data | range | - | - |
| SYS_RAMINFO | 0x26 | free (return) | used (return) | - | - | - |
| SYS_MOUSE_INFO | 0x27 | &mouse_info | - | - | - | - |
| SYS_TRUNCATE | 0x28 | fd | new_size | - | - | - |
| SYS_TIME_MS | 0x29 | ms (return)  | - | - | - | - |
| SYS_THREAD | 0x2a | entry | pid (return) | - | - | - |
| SYS_SET_TERM | 0x2b | pid | term | - | - | - |
