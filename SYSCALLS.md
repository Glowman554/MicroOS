# Syscalls

| Syscall | eax | ebx | ecx | edx | esi | edi |
|---------|-----|-----|-----|-----|-----|-----|
| SYS_OPEN | 0x1 | path | flags | fd (return) | - | - |
| SYS_CLOSE | 0x2 | fd | - | - | - | - | - |
| SYS_READ | 0x3 | fd | buf | count | offset | - |
| SYS_WRITE | 0x4 | fd | buf | count | offset | - |
| SYS_FILESIZE | 0x5 | fd | size (return) | - | - | - | - |
| SYS_ASYNC_GETC | 0x6 | c (return) | - | - | - | - | - |
| SYS_EXIT | 0x7 | status | - | - | - | - | - |