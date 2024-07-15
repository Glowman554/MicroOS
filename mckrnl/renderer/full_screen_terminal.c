#include <renderer/full_screen_terminal.h>
#include <renderer/text_mode_emulation.h>
#include <renderer/text_console.h>

#include <utils/multiboot.h>

#include <stdint.h>
#include <utils/io.h>
#include <stdio.h>
#include <string.h>
#include <config.h>

int cursor_x = 0;
int cursor_y = 0;

uint32_t fst_bgcolor = 0x0;
uint32_t fst_color = 0xffffffff;

bool full_screen_terminal_driver_is_present(driver_t* driver) {
	return true;
}

char* full_screen_terminal_driver_get_device_name(driver_t* driver) {
	return "full_screen_terminal";
}

void full_screen_terminal_clear() {
	cursor_x = 0;
	cursor_y = 0;
	memset((void*) (uint32_t) global_multiboot_info->fb_addr, 0x00, global_multiboot_info->fb_pitch * global_multiboot_info->fb_height);
}

void full_screen_terminal_driver_init(driver_t* driver) {
	full_screen_terminal_clear();
	printf_driver = (char_output_driver_t*) driver;
	global_char_output_driver = (char_output_driver_t*) driver;
}

void full_screen_terminal_driver_putc(char_output_driver_t* driver, char c) {
	if(c == 0) {
		return;
	}

	if(c == '\b') {
		draw_char(cursor_x, cursor_y, ' ', fst_color, fst_bgcolor);

		if (cursor_x - 16 >= 0) {			
			cursor_x -= 8;
		}
		return;
	}

	if(cursor_x + 16 > global_multiboot_info->fb_width || c == '\n') {
		cursor_x = 0;
		cursor_y += 16;
	} else {
		cursor_x += 8;
	}

	if (cursor_y + 16 > global_multiboot_info->fb_height) {
		memcpy((void*) (uint32_t) global_multiboot_info->fb_addr, (void*)((uint32_t) global_multiboot_info->fb_addr + (16 * global_multiboot_info->fb_pitch)), (global_multiboot_info->fb_width * 4 * (global_multiboot_info->fb_height - 16)));
		memset((void*) ((uint32_t) global_multiboot_info->fb_addr + ((global_multiboot_info->fb_width * 4) * (global_multiboot_info->fb_height - 16))), 0, (global_multiboot_info->fb_width * 4 * 16));
		cursor_y -= 16;
	}

	if(c == '\n') {
		return;
	}

	if (c >= 20 && c <= 126) {
		draw_char(cursor_x, cursor_y, c, fst_color, fst_bgcolor);
	}
}

int full_screen_terminal_driver_vmode(char_output_driver_t* driver) {
	return CUSTOM;
}



void full_screen_terminal_set_color(char_output_driver_t* driver, char* color, bool background) {
	int i;
	for (i = 0; i < 16; i++) {
		if (strcmp(color_table[i], color) == 0) {
			break;
		}
	}

	if (background) {
		fst_bgcolor = color_translation_table[i];
	} else {
		fst_color = color_translation_table[i];
	}
}

char_output_driver_t full_screen_terminal_driver = {
	.driver = {
		.is_device_present = full_screen_terminal_driver_is_present,
		.get_device_name = full_screen_terminal_driver_get_device_name,
		.init = full_screen_terminal_driver_init
	},
	.putc = full_screen_terminal_driver_putc,
	.vmode = full_screen_terminal_driver_vmode,
	.set_color = full_screen_terminal_set_color
};