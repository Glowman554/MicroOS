;!/scripts/asmlaunch-libc.msh

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


main:
    ; argc
    mov ebx, [esp + 4]
    ; argv
    mov esi, [esp + 8]
  
;     mov edi, 0
; .printargs:
;     cmp edi, ebx
;     jge .doneargs

;     mov eax, [esi + edi*4]
;     push eax
;     push edi
;     push debugmsg
;     call printf
;     add esp, 12

;     inc edi
;     jmp .printargs

; .doneargs:

    cmp ebx, 2
    jne .errorargs

    mov eax, [esi + 4] ; argv[1]
    mov [filename], eax

    push filemode
    push dword [filename]
    call fopen
    add esp, 8

    test eax, eax
    jz .erroropen

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

    mov dword [filebuf], eax



    push dword [fileptr]    ; fileptr
    push dword 1            ; nmeb
    push dword [filesz]     ; size
    push dword [filebuf]    ; ptr
    call fread
    add esp, 16

    push dword [fileptr]    ; fileptr
    call fclose
    add esp, 4



    mov ebx, 0
    mov esi, [filesz]
.countloop:
    cmp ebx, esi
    jge .out                ; if we've reached the end of the buffer, we're done

    mov eax, [filebuf]
    mov al, byte [eax + ebx]

    inc dword [chars]       ; count the character

    cmp al, 10              ; newline
    jne .skipnewline
    inc dword [lines]       ; count the line
.skipnewline:

    cmp al, 32              ; space
    je .foundspace
    cmp al, 9               ; tab
    je .foundspace
    cmp al, 13              ; carriage return
    je .foundspace
    cmp al, 11              ; vertical tab
    je .foundspace
    cmp al, 10              ; newline
    je .foundspace

    jmp .skipspace

.foundspace:
    mov byte [inword], 0
    jmp .next

.skipspace:
    cmp byte [inword], 0
    je .foundword
    jmp .next

.foundword:
    inc dword [words]       ; count the word
    mov byte [inword], 1

.next:

    add ebx, 1
    jmp .countloop

.out:

    push dword [filename]
    push dword [chars]
    push dword [words]
    push dword [lines]
    push outmsg
    call printf
    add esp, 20

    push dword [filebuf]     ; ptr
    call free
    add esp, 4

    mov eax, 0
    ret


.errorargs:
    push errormsgarg
    call printf
    add esp, 4

    mov eax, 1
    ret

.erroropen:
    push dword [filename]
    push errormsgopn
    call printf
    add esp, 8

    mov eax, 1
    ret

section '.data' writeable

errormsgarg db "Usage: wc <file>", 10, 0
errormsgopn db "Failed to open file %s", 10, 0

debugmsg    db "argv[%d]: %s", 10, 0
outmsg      db "%d %d %d %s", 10, 0

filemode    db "r", 0

filename    dd 0        ; ptr to filename

fileptr     dd 0
filesz      dd 0
filebuf     dd 0        ; ptr to file contents

lines       dd 0
words       dd 0
chars       dd 0
inword      db 0