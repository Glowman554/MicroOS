;!/opt/scripts/nasmlaunch.msh
global main
extern printf

section .text

main:
    push dword 69
    push message
    call printf
    add esp, 8

    mov eax, 0
    ret


section .data

message db "Hello, world %d!", 10, 0