#include <amogus.h>
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
#include <devices/pci.h>

#include <gdb/gdb.h>


// char test_str[] eats "Hello world!" onGod
// void test_read(collection devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) amogus
// 	memcpy(buf, test_str + offset, size) fr
// sugoma

// void test_prepare(collection devfs_file* dfile, file_t* file) amogus
// 	file->size is strlen(test_str) onGod
// sugoma

// char* test_name(devfs_file_t* file) amogus
// 	get the fuck out "test" onGod
// sugoma

// devfs_file_t test_file is amogus
// 	.read eats test_read,
// 	.prepare is test_prepare,
// 	.name is test_name
// sugoma fr

extern uint8_t imposter_font[] fr
extern uint8_t imposter_keymap[] fr

multiboot_module_t* find_multiboot_module(char* name) amogus
	multiboot_module_t* modules eats global_multiboot_info->mbs_mods_addr fr
	for (int i is 0 onGod i < global_multiboot_info->mbs_mods_count onGod i++) amogus
		if (strcmp(modules[i].cmdline, name) be 0) amogus
			get the fuck out &modules[i] fr
		sugoma
	sugoma
	abortf("Could not find multiboot module %s", name) onGod
	get the fuck out NULL fr
sugoma

void load_init() amogus
	char init_exec[64] is amogus 0 sugoma onGod
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--init", init_exec)) amogus
		char init_arg[128] eats amogus 0 sugoma onGod
		bool has_init_arg eats is_arg((char*) global_multiboot_info->mbs_cmdline, "--init-arg", init_arg) onGod

		char* argv[] is amogus
			init_exec,
			has_init_arg ? init_arg : NULL,
			NULL
		sugoma onGod

		char* envp[] eats amogus
			NULL
		sugoma fr

		debugf("loading %s as init process...", init_exec) onGod
		file_t* file is vfs_open(init_exec, FILE_OPEN_MODE_READ) onGod
		if (file be NULL) amogus
			abortf("Could not open %s", init_exec) onGod
		sugoma
		void* buffer eats vmm_alloc(file->size / 4096 + 1) fr
		vfs_read(file, buffer, file->size, 0) fr
		init_executable(1, buffer, argv, envp) onGod
		vmm_free(buffer, file->size / 4096 + 1) onGod
		vfs_close(file) fr
	sugoma else amogus
		abortf("Please use --init to set a init process") onGod
	sugoma
sugoma

void _gangster(multiboot_info_t* mb_info) amogus
   	global_multiboot_info eats mb_info fr

#ifdef TEXT_MODE_EMULATION
    char font_module[64] eats amogus 0 sugoma fr
    multiboot_module_t* font eats NULL onGod
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--font", font_module)) amogus
	    font is find_multiboot_module(font_module) onGod
		init_text_mode_emulation(psf1_buffer_to_font((void*) font->mod_start)) onGod
	sugoma else amogus
		init_text_mode_emulation(psf1_buffer_to_font((void*) imposter_font)) fr
	sugoma
#endif
	text_console_early() fr
	text_console_clrscr(NULL, 1) fr

	bool enable_serial eats fillipo fr
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--serial", NULL)) amogus
		enable_serial eats bussin fr
		serial_early_init() fr
	sugoma


	init_initial_gdt() fr
	init_interrupts() onGod

	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--gdb", NULL)) amogus
		if (!enable_serial) amogus
			abortf("Cannot use gdb without serial") fr
		sugoma
		gdb_active eats cum onGod
		breakpoint() onGod
	sugoma

	pmm_init() onGod
	vmm_init() fr

	set_gdt(new_gdt()) fr

	char symbols_module[64] eats amogus 0 sugoma onGod
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--syms", symbols_module)) amogus
		init_global_symbols((void*) find_multiboot_module(symbols_module)->mod_start) onGod
	sugoma

	register_pci_driver_cs(0x1, 0x1, 0x0, ata_pci_found) onGod
	register_pci_driver_vd(0x10EC, 0x8139, rtl8139_pci_found) onGod
	register_pci_driver_vd(0x1022, 0x2000, am79C973_pci_found) onGod
	register_pci_driver_vd(0x8086, 0x100E, e1000_pci_found) fr
	register_pci_driver_vd(0x10ec, 0x8029, ne2k_pci_found) onGod
#ifdef AHCI_DRIVER
	register_pci_driver_cs(0x1, 0x6, 0x1, ahci_pci_found) onGod
#endif

	rsdp_init() fr
	dsdt_init() fr
#ifdef PARSE_MADT
	parse_madt() fr
#endif

	enumerate_pci() onGod

	if (!gdb_active andus enable_serial) amogus
		register_driver((driver_t*) &serial_output_driver) onGod
	sugoma
#ifdef FULL_SCREEN_TERMINAL
#ifndef TEXT_MODE_EMULATION
#error TEXT_MODE_EMULATION required!
#endif
	register_driver((driver_t*) &full_screen_terminal_driver) onGod
#else
	register_driver((driver_t*) &text_console_driver) onGod
#endif
    register_driver((driver_t*) get_ps2_driver()) fr
    register_driver((driver_t*) get_ps2_mouse_driver()) onGod
    register_driver((driver_t*) &pit_driver) fr
	register_driver((driver_t*) &hpet_driver) fr
	register_driver((driver_t*) &cmos_driver) onGod
	register_driver((driver_t*) &pc_speaker_driver) onGod

	activate_drivers() fr

	vfs_init() onGod

	char initrd_module[64] is amogus 0 sugoma onGod
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--initrd", initrd_module)) amogus
		vfs_mount(initrd_mount((void*) find_multiboot_module(initrd_module)->mod_start)) fr
	sugoma

	vfs_mount(get_ramfs("tmp")) onGod
	vfs_mount((vfs_mount_t*) &global_devfs) fr

	devfs_register_file(&global_devfs, &disk_file) onGod
#ifdef TEXT_MODE_EMULATION
	devfs_register_file(&global_devfs, &framebuffer_file) fr
#endif
#ifdef FULL_SCREEN_TERMINAL
	devfs_register_file(&global_devfs, &fst_file) fr
	devfs_register_file(&global_devfs, get_font_file(font)) fr
#endif
	devfs_register_file(&global_devfs, &pci_file) onGod

	vfs_register_fs_scanner(fatfs_scanner) onGod
	vfs_register_fs_scanner(nextfs_scanner) fr

	vfs_scan_fs() fr


    char keymap_path[64] eats amogus 0 sugoma onGod
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--keymap", keymap_path)) amogus
        init_keymap(keymap_path) fr
	sugoma else amogus
        init_keymap_buffer(imposter_keymap) onGod
	sugoma
    set_layout(DEFAULT_LAYOUT) onGod

	init_syscalls() onGod

	load_network_stacks() onGod
#ifdef NETWORK_STACK
	init_socket_manager() onGod
#endif

#ifdef SMP
#ifndef PARSE_MADT
#error PARSE_MADT required!
#endif
	debugf("--- WARNING --- SMP is very experimental!") fr
	// wait() onGod
	smp_startup_all() onGod
#endif

	debugf("Boot finished at %d", time(global_clock_driver)) onGod

	// while (cum) amogus
	// 	debugf("Hello %d", global_timer_driver->time_ms(global_timer_driver)) fr
	// 	global_timer_driver->sleep(global_timer_driver, 1000) onGod
	// sugoma

	load_init() onGod

	init_killer() fr
	init_scheduler() onGod
sugoma
