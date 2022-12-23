#include "driver/driver.h"
#include <renderer/text_console.h>
#include <stdio.h>

#include <interrupts/gdt.h>
#include <interrupts/interrupts.h>
#include <syscall/syscalls.h>
#include <scheduler/scheduler.h>
#include <scheduler/killer.h>
#include <memory/pmm.h>
#include <memory/vmm.h>

#include <driver/input/ps2_keyboard.h>
#include <driver/disk/ata.h>
#include <driver/output/serial.h>
#include <driver/clock/cmos.h>
#include <driver/timer/pit.h>
#include <driver/pci/pci.h>
#include <driver/acpi/rsdp.h>
#include <driver/acpi/dsdt.h>
#include <driver/clock_driver.h>
#include <driver/nic_driver.h>
#include <driver/network/rtl8139.h>
#include <driver/network/am79C973.h>
#include <driver/network/e1000.h>
#include <driver/sound/pc_speaker.h>

#include <fs/initrd.h>
#include <fs/fatfs/fatdrv.h>

#include <utils/multiboot.h>
#include <utils/string.h>

#include <utils/argparser.h>

#include <net/socket_manager.h>

void _main(multiboot_info_t* mb_info) {	
	text_console_clrscr();

	debugf("Setting global multiboot info to %p", mb_info);
	global_multiboot_info = mb_info;

	init_gdt();
	init_interrupts();

	pmm_init();
	vmm_init();

	register_pci_driver_cs(0x1, 0x1, 0x0, ata_pci_found);
	register_pci_driver_vd(0x10EC, 0x8139, rtl8139_pci_found);
	register_pci_driver_vd(0x1022, 0x2000, am79C973_pci_found);
	register_pci_driver_vd(0x8086, 0x100E, e1000_pci_found);

	rsdp_init();
	dsdt_init();

	enumerate_pci();

	register_driver((driver_t*) &serial_output_driver);
	register_driver((driver_t*) &text_console_driver);
	register_driver((driver_t*) get_ps2_driver());
	register_driver((driver_t*) &pit_driver);
	register_driver((driver_t*) &cmos_driver);
	register_driver((driver_t*) &pc_speaker_driver);

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

	load_network_stacks();
	init_socket_manager();
	
	debugf("Boot finished at %d", time(global_clock_driver));

	char init_exec[64] = { 0 };
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--init", init_exec)) {
		char* argv[] = {
			init_exec,
			NULL
		};

		char* envp[] = {
			NULL
		};

		debugf("loading %s as init process...", init_exec);
		file_t* file = vfs_open(init_exec, FILE_OPEN_MODE_READ);
		if (file == NULL) {
			abortf("Could not open %s", init_exec);
		}
		void* buffer = vmm_alloc(file->size / 4096 + 1);
		vfs_read(file, buffer, file->size, 0);
		init_elf(buffer, argv, envp);
		vmm_free(buffer, file->size / 4096 + 1);
		vfs_close(file);
	} else {
		abortf("Please use --init to set a init process");
	}

	init_killer();
	init_scheduler();
}
