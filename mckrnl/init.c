#include <renderer/text_console.h>
#include <stdio.h>

#include <interrupts/gdt.h>
#include <interrupts/interrupts.h>
#include <syscall/syscalls.h>
#include <scheduler/scheduler.h>
#include <memory/pmm.h>
#include <memory/vmm.h>

#include <driver/output/serial.h>
#include <driver/input/ps2_keyboard.h>
#include <driver/disk/ata.h>
#include <driver/timer/pit.h>

#include <fs/initrd.h>
#include <fs/fatfs/fatdrv.h>

#include <utils/multiboot.h>
#include <utils/string.h>

#include <utils/argparser.h>

void main(multiboot_info_t* mb_info) {	
	text_console_clrscr();

	debugf("Setting global multiboot info to %p", mb_info);
	global_multiboot_info = mb_info;

	init_gdt();
	init_interrupts();

	pmm_init();
	vmm_init();

	register_driver((driver_t*) &serial_output_driver);
	register_driver((driver_t*) &text_console_driver);
	register_driver((driver_t*) get_ps2_driver());

	register_driver((driver_t*) get_ata_driver(true, 0x1F0, "ata0_master"));
	register_driver((driver_t*) get_ata_driver(false, 0x1F0, "ata0_slave"));
	register_driver((driver_t*) get_ata_driver(true, 0x170, "ata1_master"));
	register_driver((driver_t*) get_ata_driver(false, 0x170, "ata1_slave"));

	register_driver((driver_t*) &pit_driver);

	activate_drivers();

	vfs_init();

	multiboot_module_t* modules = global_multiboot_info->mbs_mods_addr;
	char initrd_module[64] = { 0 };
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--initrd", initrd_module)) {
		for (int i = 0; i < global_multiboot_info->mbs_mods_count; i++) {
			if (strcmp(modules[i].cmdline, initrd_module) == 0) {
				vfs_mount(initrd_mount((void*) modules[i].mod_start));
			}
		}
	}

	vfs_register_fs_scanner(fatfs_scanner);

	vfs_scan_fs();

	init_syscalls();

	printf("MicroOS Copyright (C) 2022 Glowman554\n\n");

	init_scheduler();
}