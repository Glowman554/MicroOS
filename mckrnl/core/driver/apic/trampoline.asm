[org 0x8000]
[bits 16]

_entry:
	cli
	cld
	
	lgdt [gdt_descriptor]

	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp codeseg:protected_mode

[bits 32]
protected_mode:
	mov ax, dataseg
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov esp, [trampoline_data + ap_info.stack]
	xor ebp, ebp
	
	mov eax, [trampoline_data + ap_info.entry]
	mov [trampoline_data + ap_info.done], byte 1 ; tell bsp that we are done
	call eax

	hlt

gdt_nulldesc:
	dd 0
	dd 0	
gdt_codedesc:
	dw 0xFFFF			; Limit
	dw 0x0000			; Base (low)
	db 0x00				; Base (medium)
	db 10011010b		; Flags
	db 11001111b		; Flags + Upper Limit
	db 0x00				; Base (high)
gdt_datadesc:
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 10010010b
	db 11001111b
	db 0x00

gdt_end:

gdt_descriptor:
	gdt_size: 
		dw gdt_end - gdt_nulldesc - 1
		dq gdt_nulldesc

codeseg equ gdt_codedesc - gdt_nulldesc
dataseg equ gdt_datadesc - gdt_nulldesc

struc ap_info
	.entry resd 1
	.stack resd 1
	.done resb 1
endstruc

times 4096 - ($ - $$) db 0

trampoline_data: