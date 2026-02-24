;!initrd:/scripts/asmlaunch-libc.msh

format ELF
public main

extrn printf
extrn fopen
extrn ftell
extrn fseek
extrn fclose
extrn fread
extrn malloc
extrn free

section '.text' executable

; Callee-saved (safe to use):   ebx esi edi ebp esp
; Caller-saved (NOT safe):      eax ecx edx

; Example reading the dev:symbols file, which contains the symbol table of the loaded task, and printing the symbols to the console. 

main:
    push filemode
    push filename
    call fopen
    add esp, 8

    test eax, eax
    jz .error_open

    mov dword [fileptr], eax



    push dword 2            ; whence (SEEK_END)
    push dword 0            ; offset
    push dword [fileptr]    ; fileptr
    call fseek
    add esp, 12


    push dword [fileptr]    ; fileptr
    call ftell
    add esp, 4

    mov dword [filesz], eax


    push dword 0            ; whence (SEEK_SET)
    push dword 0            ; offset
    push dword [fileptr]    ; fileptr
    call fseek
    add esp, 12



    push dword [filesz]     ; size
    call malloc
    add esp, 4

    mov dword [symbuf], eax



    push dword [fileptr]    ; fileptr
    push dword 1            ; nmeb
    push dword [filesz]     ; size
    push dword [symbuf]     ; ptr
    call fread
    add esp, 16

    push dword [fileptr]    ; fileptr
    call fclose
    add esp, 4


    push dword [filesz]
    push dword [symbuf]
    push debugbuf
    call printf
    add esp, 12

    mov ebx, 0
    mov esi, [filesz]
.printloop:
    cmp ebx, esi
    jge .out                ; if we've reached the end of the buffer, we're done

    mov eax, [symbuf]
    add eax, ebx            ; current symbol struct

    mov edx, [eax + 64]     ; symbol address
    push edx
    push eax                ; symbol name
    push outmsg
    call printf
    add esp, 12

    add ebx, 68             ; struct size
    jmp .printloop

.out:

    push dword [symbuf]     ; ptr
    call free
    add esp, 4

    mov eax, 0
    ret

.error_open:
    push errormsg
    call printf
    add esp, 4

    mov eax, 1
    ret

section '.data' writeable

errormsg    db "Failed to open symbols file", 10, 0
debugbuf    db "Buffer at: 0x%x, size: %d", 10, 0
outmsg      db "%s: 0x%x", 10, 0

filename    db "dev:symbols", 0
filemode    db "r", 0

fileptr     dd 0
filesz      dd 0
symbuf      dd 0