#include "include/config.h"

.section multiboot
#define MB_MAGIC 0x1badb002

#ifdef TEXT_MODE_EMULATION
#define MB_FLAGS (1 << 2)
#else
#define MB_FLAGS 0
#endif

#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)

// Der Multiboot-Header
.align 4
.int	MB_MAGIC     // 0
.int	MB_FLAGS     // 4
.int	MB_CHECKSUM  // 8
.int    0 // unused  // 12
.int    0 // unused  // 16
.int    0 // unused  // 20
.int    0 // unused  // 24
.int    0 // unused  // 28
.int    0 // mode    // 32
.int    0 // width   // 36
.int    0 // height  // 40
.int    32 // depth  // 44


.section .text

// Init ist eine Funktion aus init.c
.extern _main

// _start muss global sein, damit der Linker es findet und als Einsprungspunkt
// benutzen kann (alle Labels, die nicht global sind, sind nur in dieser Datei
// sichtbar)
.global _start
_start:
	cli

	// Stack initialisieren
	mov $kernel_stack, %esp
	xor %ebp, %ebp

	// CPP-Code aufrufen und Multiboot-Infostruktur als Parameter uebergeben
	push %ebx
	call _main
	// Falls wir jemals aus init zurueckkommen sollten, gehen wir in eine
	// Endlosschleife

	jmp .

// 8 kB Stack fuer den Kernel. Das Label steht hinter dem freien Speicher,
// weil der Stack nach unten waechst
.section .bss
.space 8192 * 64
kernel_stack: