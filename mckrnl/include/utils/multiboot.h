#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

typedef struct multiboot_info {
 	uint32_t mbs_flags;
	uint32_t mbs_mem_lower;
	uint32_t mbs_mem_upper;
	uint32_t mbs_bootdevice;
	uint32_t mbs_cmdline;
	uint32_t mbs_mods_count;
	void* mbs_mods_addr;
	uint32_t mbs_syms[4];
	uint32_t mbs_mmap_length;
	void* mbs_mmap_addr;

    uint32_t drives_length;
    uint32_t drivers_addr;

    uint32_t rom_config_table;

    uint32_t bootloader_name;

    uint32_t apm_table;

    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    uint64_t fb_addr;
    uint32_t fb_pitch;
    uint32_t fb_width;
    uint32_t fb_height;
    uint8_t fb_bpp;
    uint8_t fb_type;

    uint8_t fb_red_mask_shift;
    uint8_t fb_red_mask_size;
    uint8_t fb_green_mask_shift;
    uint8_t fb_green_mask_size;
    uint8_t fb_blue_mask_shift;
    uint8_t fb_blue_mask_size;
} __attribute__((packed)) multiboot_info_t;

typedef struct multiboot_mmap {
	uint32_t entry_size;
	uint64_t base;
	uint64_t length;
	uint32_t type;
} __attribute__((packed)) multiboot_mmap_t;

typedef struct multiboot_module {
	uint32_t mod_start;
	uint32_t mod_end;
	char* cmdline;
	uint32_t reserved;
} __attribute__((packed)) multiboot_module_t;


extern multiboot_info_t* global_multiboot_info;

#endif
