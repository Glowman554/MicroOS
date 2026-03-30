#include <fcntl.h>
#include <stdio.h>
#include <reent.h>

#include "config.h"

extern void exit(int status);
extern int main(int argc, char **argv, char **envp);
extern char **environ;
extern void __sinit(struct _reent *);
extern struct _reent *_impure_ptr;

extern char __bss_start;
extern char _end;

void _start(void) {
    char *bss = &__bss_start;
    char *bss_end = &_end;
    while (bss < bss_end) {
        *bss++ = 0;
    }
    
    char** argv = (char**)0;
    char** envp = (char**)0;
    
    __asm__ volatile(
        "int $0x30"
        : "=c"(argv)
        : "a"(SYS_ENV_ID), "b"(SYS_GET_ARGV_ID)
    );
    
    __asm__ volatile(
        "int $0x30"
        : "=c"(envp)
        : "a"(SYS_ENV_ID), "b"(SYS_GET_ENVP_ID)
    );
    
    environ = envp;

    int argc = 0;
    while (argv[argc] != (char*)0) {
        argc++;
    }
    
    __sinit(_impure_ptr);    
    
    exit(main(argc, argv, envp));
    
    while (1) { }
}