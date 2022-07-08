# Syscalls

| Syscall | eax | ebx | ecx | edx | esi | edi |
|---------|-----|-----|-----|-----|-----|-----|
| SYS_OPEN | 0x1 | path | flags | fd (return) | - | - |
| SYS_CLOSE | 0x2 | fd | - | - | - | - | - |
| SYS_READ | 0x3 | fd | buf | count | offset | - |
| SYS_WRITE | 0x4 | fd | buf | count | offset | - |
| SYS_FILESIZE | 0x5 | fd | size (return) | - | - | - | - |
| SYS_DELETE | 0x6 | fd | - | - | - | - | - |
| SYS_MKDIR | 0x7 | path | - | - | - | - | - |
| SYS_DIR_AT | 0x8 | path | idx | dir_t* | - | - | - |
| SYS_TOCUH | 0x9 | path | - | - | - | - | - |
| SYS_DELETE_DIR | 0xa | path | - | - | - | - | - |
| --- | --- | --- | --- | --- | --- | --- | --- |
| SYS_ASYNC_GETC | 0xb | c (return) | - | - | - | - | - |
| SYS_EXIT | 0xc | status | - | - | - | - | - |
| SYS_MMAP | 0xd | addr | - | - | - | - | - |

| SYS_SPAWN | 0xe | path | argv | envp | pid (return) | - | - |
| SYS_PROC_INFO | 0xf | pid | running (return) | - | - | - | - | - |
| SYS_YIELD | 0x10 | - | - | - | - | - | - |
| SYS_ENV | 0x11 | mode | void* data (return?) | - | - | - | - | - |