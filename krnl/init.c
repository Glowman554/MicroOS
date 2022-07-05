#include <renderer/text_console.h>
#include <stdio.h>

#include <interrupts/gdt.h>
#include <interrupts/interrupts.h>
#include <syscall/syscalls.h>
#include <scheduler/scheduler.h>
#include <memory/pmm.h>
#include <memory/vmm.h>

#include <utils/multiboot.h>

void init(multiboot_info_t* mb_info) {	
	text_console_clrscr();

	printf("MicroOS Copyright (C) 2022 Glowman554\n\n");

	debugf("Setting global multiboot info to %p\n", mb_info);
	global_multiboot_info = mb_info;

	init_gdt();
	init_interrupts();

	pmm_init();
	vmm_init();

	init_syscalls();

	init_scheduler();
}