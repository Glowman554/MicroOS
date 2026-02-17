;!initrd:/scripts/asmlaunch-libc.msh

format ELF
public main

extrn printf

section '.text' executable

main:
    push dword 69
    push message
    call printf
    add esp, 8

    mov eax, 0
    ret

section '.data' writeable

message db "Hello, world %d!", 10, 0
