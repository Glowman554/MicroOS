format ELF executable at 0xB0000000
entry _start

segment readable executable

_start:
	mov ecx, idle
	call strlen
	mov edx, eax

	mov eax, 0x4
	mov ebx, 1
	mov ecx, idle
	mov esi, 0
	int 0x30

_loop:
	; sleep
	mov eax, 0x12
	mov ebx, 8
	mov ecx, 1000
	int 0x30

	; yield
	mov eax, 0x11
	int 0x30

	jmp _loop

; buf in ecx
strlen:
	mov eax, ecx
.loop:
	cmp [eax], byte 0
	je .out
	inc eax
	jmp .loop
.out:
	sub eax, ecx
	ret

idle: db "Starting idle task", 10, 0