.section multiboot
#define MB_MAGIC 0x1badb002
#define MB_FLAGS 0x0
#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)

// Der Multiboot-Header
.align 4
.int	MB_MAGIC
.int	MB_FLAGS
.int	MB_CHECKSUM

.section .text

// Init ist eine Funktion aus init.c
.extern init

// _start muss global sein, damit der Linker es findet und als Einsprungspunkt
// benutzen kann (alle Labels, die nicht global sind, sind nur in dieser Datei
// sichtbar)
.global _start
_start:
	cli

	// Stack initialisieren
	mov $kernel_stack, %esp

	// CPP-Code aufrufen und Multiboot-Infostruktur als Parameter uebergeben
	push %ebx
	call init
	// Falls wir jemals aus init zurueckkommen sollten, gehen wir in eine
	// Endlosschleife

	jmp .

// 8 kB Stack fuer den Kernel. Das Label steht hinter dem freien Speicher,
// weil der Stack nach unten waechst
.section .bss
.space 8192
kernel_stack: