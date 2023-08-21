#include <driver/driver.h>
#include <renderer/text_console.h>
#include <renderer/psf1_font.h>
#include <renderer/text_mode_emulation.h>
#include <stdio.h>

#include <interrupts/gdt.h>
#include <interrupts/interrupts.h>
#include <syscall/syscalls.h>
#include <scheduler/scheduler.h>
#include <scheduler/killer.h>
#include <memory/pmm.h>
#include <memory/vmm.h>

#include <driver/input/ps2_keyboard.h>
#include <driver/input/ps2_mouse.h>
#include <driver/disk/ata.h>
#include <driver/output/serial.h>
#include <driver/clock/cmos.h>
#include <driver/timer/pit.h>
#include <driver/timer/hpet.h>
#include <driver/pci/pci.h>
#include <driver/acpi/rsdp.h>
#include <driver/acpi/dsdt.h>
#include <driver/acpi/madt.h>
#include <driver/apic/smp.h>
#include <driver/clock_driver.h>
#include <driver/nic_driver.h>
#include <driver/network/rtl8139.h>
#include <driver/network/am79C973.h>
#include <driver/network/e1000.h>
#include <driver/sound/pc_speaker.h>

#include <fs/initrd.h>
#include <fs/devfs.h>
#include <fs/fatfs/fatdrv.h>
#include <fs/ramfs.h>

#include <utils/multiboot.h>
#include <utils/string.h>
#include <utils/trace.h>

#include <utils/argparser.h>

#include <net/socket_manager.h>

#include <devices/disk.h>

// char test_str[] = "Hello world!";
// void test_read(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
// 	memcpy(buf, test_str + offset, size);
// }

// void test_prepare(struct devfs_file* dfile, file_t* file) {
// 	file->size = strlen(test_str);
// }

// char* test_name(devfs_file_t* file) {
// 	return "test";
// }

// devfs_file_t test_file = {
// 	.read = test_read,
// 	.prepare = test_prepare,
// 	.name = test_name
// };

void* find_multiboot_module(char* name) {
	multiboot_module_t* modules = global_multiboot_info->mbs_mods_addr;
	for (int i = 0; i < global_multiboot_info->mbs_mods_count; i++) {
		if (strcmp(modules[i].cmdline, name) == 0) {
			return (void*) modules[i].mod_start;
		}
	}
	abortf("Could not find multiboot module %s", name);
	return NULL;
}



void _main(multiboot_info_t* mb_info) {	
   	global_multiboot_info = mb_info;

#ifdef TEXT_MODE_EMULATION
    char font_module[64] = { 0 };
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--font", font_module)) {
		init_text_mode_emulation(psf1_buffer_to_font(find_multiboot_module(font_module)));
	}
#endif
	text_console_clrscr();

	init_initial_gdt();
	init_interrupts();

	pmm_init();
	vmm_init();

	set_gdt(new_gdt());

	char symbols_module[64] = { 0 };
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--syms", symbols_module)) {
		init_global_symbols(find_multiboot_module(symbols_module));
	}

	register_pci_driver_cs(0x1, 0x1, 0x0, ata_pci_found);
	register_pci_driver_vd(0x10EC, 0x8139, rtl8139_pci_found);
	register_pci_driver_vd(0x1022, 0x2000, am79C973_pci_found);
	register_pci_driver_vd(0x8086, 0x100E, e1000_pci_found);

	rsdp_init();
	dsdt_init();
#ifdef PARSE_MADT
	parse_madt();
#endif

	enumerate_pci();

	register_driver((driver_t*) &serial_output_driver);
	register_driver((driver_t*) &text_console_driver);
    register_driver((driver_t*) get_ps2_driver());
    register_driver((driver_t*) get_ps2_mouse_driver());
    register_driver((driver_t*) &pit_driver);
	register_driver((driver_t*) &hpet_driver);
	register_driver((driver_t*) &cmos_driver);
	register_driver((driver_t*) &pc_speaker_driver);

	activate_drivers();

	vfs_init();

	char initrd_module[64] = { 0 };
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--initrd", initrd_module)) {
		vfs_mount(initrd_mount((void*) find_multiboot_module(initrd_module)));
	}

	vfs_mount(get_ramfs("tmp"));
	vfs_mount((vfs_mount_t*) &global_devfs);

	devfs_register_file(&global_devfs, &disk_file);

	vfs_register_fs_scanner(fatfs_scanner);

	vfs_scan_fs();
    

    char keymap_path[64] = { 0 };
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--keymap", keymap_path)) {
        init_keymap(keymap_path);
        set_layout(DEFAULT_LAYOUT);
	}

	init_syscalls();

	load_network_stacks();
	init_socket_manager();

#ifdef SMP
#ifndef PARSE_MADT
#error PARSE_MADT required!
#endif
	debugf("--- WARNING --- SMP is very experimantel!");
	// wait();
	smp_startup_all();
#endif	
	
	debugf("Boot finished at %d", time(global_clock_driver));

	// while (true) {
	// 	debugf("Hello %d", global_timer_driver->time_ms(global_timer_driver));
	// 	global_timer_driver->sleep(global_timer_driver, 1000);
	// }

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
