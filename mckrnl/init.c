#include <driver/driver.h>
#include <renderer/text_console.h>
#include <renderer/psf1_font.h>
#include <renderer/text_mode_emulation.h>
#include <renderer/full_screen_terminal.h>
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
#include <driver/disk/ahci.h>
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
#include <driver/network/ne2k.h>
#include <driver/sound/pc_speaker.h>

#include <fs/initrd.h>
#include <fs/devfs.h>
#include <fs/fatfs/fatdrv.h>
#include <fs/nextfs.h>
#include <fs/ramfs.h>

#include <utils/multiboot.h>
#include <utils/string.h>
#include <utils/trace.h>

#include <utils/argparser.h>

#include <net/socket_manager.h>

#include <devices/disk.h>
#include <devices/framebuffer.h>
#include <devices/fst.h>
#include <devices/font.h>

#include <gdb/gdb.h>


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

multiboot_module_t* find_multiboot_module(char* name) {
	multiboot_module_t* modules = global_multiboot_info->mbs_mods_addr;
	for (int i = 0; i < global_multiboot_info->mbs_mods_count; i++) {
		if (strcmp(modules[i].cmdline, name) == 0) {
			return &modules[i];
		}
	}
	abortf("Could not find multiboot module %s", name);
	return NULL;
}

void load_init() {
	char init_exec[64] = { 0 };
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--init", init_exec)) {
		char init_arg[128] = { 0 };
		bool has_init_arg = is_arg((char*) global_multiboot_info->mbs_cmdline, "--init-arg", init_arg);

		char* argv[] = {
			init_exec,
			has_init_arg ? init_arg : NULL,
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
		init_executable(1, buffer, argv, envp);
		vmm_free(buffer, file->size / 4096 + 1);
		vfs_close(file);
	} else {
		abortf("Please use --init to set a init process");
	}
}

void _main(multiboot_info_t* mb_info) {
   	global_multiboot_info = mb_info;

#ifdef TEXT_MODE_EMULATION
    char font_module[64] = { 0 };
    multiboot_module_t* font = NULL;
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--font", font_module)) {
	    font = find_multiboot_module(font_module);
		init_text_mode_emulation(psf1_buffer_to_font((void*) font->mod_start));
	}
#endif
	text_console_early();
	text_console_clrscr(NULL, 1);

	bool enable_serial = false;
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--serial", NULL)) {
		enable_serial = true;
		serial_early_init();
	}


	init_initial_gdt();
	init_interrupts();

	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--gdb", NULL)) {
		if (!enable_serial) {
			abortf("Cannot use gdb without serial");
		}
		gdb_active = true;
		breakpoint();
	}

	pmm_init();
	vmm_init();

	set_gdt(new_gdt());

	char symbols_module[64] = { 0 };
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--syms", symbols_module)) {
		init_global_symbols((void*) find_multiboot_module(symbols_module)->mod_start);
	}

	register_pci_driver_cs(0x1, 0x1, 0x0, ata_pci_found);
	register_pci_driver_vd(0x10EC, 0x8139, rtl8139_pci_found);
	register_pci_driver_vd(0x1022, 0x2000, am79C973_pci_found);
	register_pci_driver_vd(0x8086, 0x100E, e1000_pci_found);
	register_pci_driver_vd(0x10ec, 0x8029, ne2k_pci_found);
#ifdef AHCI_DRIVER
	register_pci_driver_cs(0x1, 0x6, 0x1, ahci_pci_found);
#endif

	rsdp_init();
	dsdt_init();
#ifdef PARSE_MADT
	parse_madt();
#endif

	enumerate_pci();

	if (!gdb_active && enable_serial) {
		register_driver((driver_t*) &serial_output_driver);
	}
#ifdef FULL_SCREEN_TERMINAL
#ifndef TEXT_MODE_EMULATION
#error TEXT_MODE_EMULATION required!
#endif
	register_driver((driver_t*) &full_screen_terminal_driver);
#else
	register_driver((driver_t*) &text_console_driver);
#endif
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
		vfs_mount(initrd_mount((void*) find_multiboot_module(initrd_module)->mod_start));
	}

	vfs_mount(get_ramfs("tmp"));
	vfs_mount((vfs_mount_t*) &global_devfs);

	devfs_register_file(&global_devfs, &disk_file);
#ifdef TEXT_MODE_EMULATION
	devfs_register_file(&global_devfs, &framebuffer_file);
#endif
#ifdef FULL_SCREEN_TERMINAL
	devfs_register_file(&global_devfs, &fst_file);
	devfs_register_file(&global_devfs, get_font_file(font));
#endif

	vfs_register_fs_scanner(fatfs_scanner);
	vfs_register_fs_scanner(nextfs_scanner);

	vfs_scan_fs();


    char keymap_path[64] = { 0 };
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--keymap", keymap_path)) {
        init_keymap(keymap_path);
        set_layout(DEFAULT_LAYOUT);
	}

	init_syscalls();

	load_network_stacks();
#ifdef NETWORK_STACK
	init_socket_manager();
#endif

#ifdef SMP
#ifndef PARSE_MADT
#error PARSE_MADT required!
#endif
	debugf("--- WARNING --- SMP is very experimental!");
	// wait();
	smp_startup_all();
#endif

	debugf("Boot finished at %d", time(global_clock_driver));

	// while (true) {
	// 	debugf("Hello %d", global_timer_driver->time_ms(global_timer_driver));
	// 	global_timer_driver->sleep(global_timer_driver, 1000);
	// }

	load_init();

	init_killer();
	init_scheduler();
}
