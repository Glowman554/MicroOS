#include <renderer/full_screen_terminal.h>
#include <renderer/text_mode_emulation.h>
#include <renderer/text_console.h>

#include <memory/vmm.h>
#include <stdint.h>
#include <utils/multiboot.h>

#include <utils/io.h>
#include <stdio.h>
#include <string.h>
#include <config.h>

full_screen_terminal_vterm_t full_screen_terminal_vterms[MAX_VTERM] = { 0 };

bool full_screen_terminal_driver_is_present(driver_t* driver) {
	return true;
}

char* full_screen_terminal_driver_get_device_name(driver_t* driver) {
	return "full_screen_terminal";
}

void full_screen_terminal_clear(char_output_driver_t* driver, int term) {
	full_screen_terminal_vterm_t* vterm = &full_screen_terminal_vterms[term - 1];
	vterm->x = 0;
	vterm->y = 0;

	void* buffer = (void*)(uint32_t) global_multiboot_info->fb_addr;
	if (driver->current_term != term) {
		buffer = vterm->buffer;
	}

	memset(buffer, 0x00, global_multiboot_info->fb_pitch * global_multiboot_info->fb_height);
}

void full_screen_terminal_driver_init(driver_t* driver) {
	int size = TO_PAGES(global_multiboot_info->fb_pitch * global_multiboot_info->fb_height);
	debugf("framebuffer size (pages): %d", size);

	for (int i = 0; i < MAX_VTERM; i++) {
		full_screen_terminal_vterm_t* vterm = &full_screen_terminal_vterms[i];
		vterm->buffer = vmm_alloc(size);
		vterm->color = 0xffffffff;
		vterm->bgcolor = 0;
		full_screen_terminal_clear((char_output_driver_t*) driver, i + 1);
	}

	printf_driver = (char_output_driver_t*) driver;
	if (!debugf_driver) {
		debugf_driver = (char_output_driver_t*) driver;
	}
	global_char_output_driver = (char_output_driver_t*) driver;
}

// void* memcpy32(void* dest, const void* src, int n) {
//     uint8_t *d = (uint8_t *)dest;
//     const uint8_t *s = (const uint8_t *)src;

//     int chunks = n / 4;
//     for (int i = 0; i < chunks; i++) {
//         ((uint32_t *)d)[i] = ((const uint32_t *)s)[i];
//     }

//     int remaining_bytes = n % 4;
//     if (remaining_bytes) {
//         d += chunks * 4;
//         s += chunks * 4;
//         for (int i = 0; i < remaining_bytes; i++) {
//             d[i] = s[i];
//         }
//     }

//     return dest;
// }

void full_screen_terminal_driver_putc(char_output_driver_t* driver, int term, char c) {
	if(c == 0) {
		return;
	}

	full_screen_terminal_vterm_t* vterm = &full_screen_terminal_vterms[term - 1];
	void* buffer = (void*)(uint32_t) global_multiboot_info->fb_addr;
	if (driver->current_term != term) {
		buffer = vterm->buffer;
	}

	if(c == '\b') {
		draw_char(buffer, vterm->x, vterm->y, ' ', vterm->color, vterm->bgcolor);

		if (vterm->x - 16 >= 0) {			
			vterm->x -= 8;
		}
		return;
	}

	if(vterm->x + 16 > global_multiboot_info->fb_width || c == '\n') {
		vterm->x = 0;
		vterm->y += 16;
	} else {
		vterm->x += 8;
	}

	if (vterm->y + 16 > global_multiboot_info->fb_height) {
		memcpy(buffer, buffer + (16 * global_multiboot_info->fb_pitch), (global_multiboot_info->fb_width * 4 * (global_multiboot_info->fb_height - 16)));
		memset(buffer + ((global_multiboot_info->fb_width * 4) * (global_multiboot_info->fb_height - 16)), 0, (global_multiboot_info->fb_width * 4 * 16));
		vterm->y -= 16;
	}

	if(c == '\n') {
		return;
	}

	if (c >= 20 && c <= 126) {
		draw_char(buffer, vterm->x, vterm->y, c, vterm->color, vterm->bgcolor);
	}
}

int full_screen_terminal_driver_vmode(char_output_driver_t* driver) {
	return CUSTOM;
}



void full_screen_terminal_set_color(char_output_driver_t* driver, int term, char* color, bool background) {
	full_screen_terminal_vterm_t* vterm = &full_screen_terminal_vterms[term - 1];
	int i;
	for (i = 0; i < 16; i++) {
		if (strcmp(color_table[i], color) == 0) {
			break;
		}
	}

	if (background) {
		vterm->bgcolor = color_translation_table[i];
	} else {
		vterm->color = color_translation_table[i];
	}
}

void full_screen_terminal_vpoke(char_output_driver_t* driver, int term, uint32_t offset, uint8_t* val, uint32_t range) {
	if (driver->current_term == term) {
    	memcpy((void*)(uint32_t)global_multiboot_info->fb_addr + offset, val, range);
	} else {
    	memcpy(full_screen_terminal_vterms[term - 1].buffer + offset, val, range);
	}
    EMU_UPDATE();
}

void full_screen_terminal_vpeek(char_output_driver_t* driver, int term, uint32_t offset, uint8_t* val, uint32_t range) {
	if (driver->current_term == term) {
    	memcpy(val, (void*)(uint32_t)global_multiboot_info->fb_addr + offset, range);
	} else {
    	memcpy(val, full_screen_terminal_vterms[term - 1].buffer + offset, range);
	}
}


void full_screen_terminal_vcursor(char_output_driver_t* driver, int term, int x, int y) {
	full_screen_terminal_vterm_t* vterm = &full_screen_terminal_vterms[term - 1];
	vterm->x = x * 16;
	vterm->y = y * 8;
}

void  full_screen_terminal_vcursor_get(char_output_driver_t* driver, int term, int* x, int* y) {
	full_screen_terminal_vterm_t* vterm = &full_screen_terminal_vterms[term - 1];
	*x = vterm->x / 16;
	*y = vterm->y / 8;
}


void full_screen_terminal_vterm(char_output_driver_t* driver, int term) {
	full_screen_terminal_vterm_t* new = &full_screen_terminal_vterms[term - 1];
	full_screen_terminal_vterm_t* old = &full_screen_terminal_vterms[driver->current_term - 1];

	memcpy(old->buffer, (void*)(uint32_t) global_multiboot_info->fb_addr, global_multiboot_info->fb_pitch * global_multiboot_info->fb_height);
	memcpy((void*)(uint32_t) global_multiboot_info->fb_addr, new->buffer, global_multiboot_info->fb_pitch * global_multiboot_info->fb_height);

	driver->current_term = term;
}


char_output_driver_t full_screen_terminal_driver = {
	.driver = {
		.is_device_present = full_screen_terminal_driver_is_present,
		.get_device_name = full_screen_terminal_driver_get_device_name,
		.init = full_screen_terminal_driver_init
	},
	.current_term = 1,
	.putc = full_screen_terminal_driver_putc,
	.vmode = full_screen_terminal_driver_vmode,
	.vpoke = full_screen_terminal_vpoke,
    .vpeek = full_screen_terminal_vpeek,
	.vterm = full_screen_terminal_vterm,
	.vcursor = full_screen_terminal_vcursor,
	.vcursor_get = full_screen_terminal_vcursor_get,
	.set_color = full_screen_terminal_set_color
};