#include <driver/input/ps2_keyboard.h>

#include <utils/io.h>
#include <utils/argparser.h>
#include <utils/multiboot.h>
#include <interrupts/interrupts.h>
#include <stdio.h>
#include <string.h>
#include <memory/vmm.h>
#include <assert.h>

// special_driver_data: byte 0 == char, byte 1 == bool special_key_next, byte 2 == bool special_code, byte 3 - 1023 == keymap path

#define KBD_CHAR 0
#define KBD_SPECIAL_KEY_NEXT 1
#define KBD_SPECIAL_CODE 2
#define KBD_KEYMAP_PATH 3
#define KBD_SPECIAL_KEYS 1024

bool ps2_keyboard_is_device_present(driver_t* driver) {
	return true;
}

char* ps2_keyboard_get_device_name(driver_t* driver) {
	return "ps2_keyboard";
}

cpu_registers_t* ps2_keyboard_interrupt_handler(cpu_registers_t* registers, void* driver) {
	driver_t* ps2_keyboard = (driver_t*) driver;

	bool* special_key_next = (bool*) &((uint8_t*) ps2_keyboard->driver_specific_data)[KBD_SPECIAL_KEY_NEXT];
	uint8_t* special_code = (uint8_t*) &((uint8_t*) ps2_keyboard->driver_specific_data)[KBD_SPECIAL_CODE];

	special_keys_down_t* special_keys_down = (special_keys_down_t*) &((uint8_t*) ps2_keyboard->driver_specific_data)[KBD_SPECIAL_KEYS];

	uint8_t key = inb(DATA_PORT);

	if (*special_key_next) {
		switch (*special_code) {
			case 0xE0:
				switch (key) {
					case 0x38: //Right alt down
						special_keys_down->right_alt = true;
						break;
					case 0xB8: //Right alt up
						special_keys_down->right_alt = false;
						break;
					case 0x1D: //Right ctrl down
						special_keys_down->right_ctrl = true;
						break;
					case 0x9D: //Right ctrl up
						special_keys_down->right_ctrl = false;
						break;
					case 0x48: //Up arrow down
						special_keys_down->up_arrow = true;
						break;
					case 0xC8: //Up arrow up
						special_keys_down->up_arrow = false;
						break;
					case 0x50: //Down arrow down
						special_keys_down->down_arrow = true;
						break;
					case 0xD0: //Down arrow up
						special_keys_down->down_arrow = false;
						break;
					case 0x4B: //Left arrow down
						special_keys_down->left_arrow = true;
						break;
					case 0xCB: //Left arrow down
						special_keys_down->left_arrow = false;
						break;
					case 0x4D: //Right arrow down
						special_keys_down->right_arrow = true;
						break;
					case 0xCD: //Right arrow up
						special_keys_down->right_arrow = false;
						break;
				}
				break;

			case 0x3A:
				switch (key) {
					case 0xBA: //Caps lock toggle
						special_keys_down->caps_lock = !special_keys_down->caps_lock;
						break;
				}
				break;
		}

		*special_key_next = false;
	} else {
		switch (key) {
			case 0xE0:
			case 0x3A:
				*special_key_next = true;
				*special_code = key;
				break;
			
			case 0x38: //Left alt down
				special_keys_down->left_alt = true;
				break;
			case 0xB8: //Left alt up
				special_keys_down->left_alt = false;
				break;
			case 0x1D: //Left ctrl down
				special_keys_down->left_ctrl = true;
				break;
			case 0x9D: //Left ctrl up
				special_keys_down->left_ctrl = false;
				break;
			case 0x2A: //Left shift down
				special_keys_down->left_shift = true;
				break;
			case 0xAA: //Left shift up
				special_keys_down->left_shift = false;
				break;
			case 0x36: //Right shift down
				special_keys_down->right_shift = true;
				break; 
			case 0xB6: //Right shift up
				special_keys_down->right_shift = false;
				break;
			default:
				{
					char c = keymap(&((char*) ps2_keyboard->driver_specific_data)[KBD_KEYMAP_PATH], key, special_keys_down);
					((char*) ps2_keyboard->driver_specific_data)[KBD_CHAR] = c;
				}
				break;
		}
	}
	return registers;
}

void ps2_keyboard_init(driver_t* driver) {
	register_interrupt_handler(0x21, ps2_keyboard_interrupt_handler, driver);

	while(inb(COMMAND_PORT) & 0x1) {
		inb(DATA_PORT);
	}
	outb(COMMAND_PORT, 0xae);
	outb(COMMAND_PORT, 0x20);
	uint8_t status = (inb(DATA_PORT) | 1) & ~0x10;

	outb(COMMAND_PORT, 0x60);
	outb(DATA_PORT, status);
	outb(DATA_PORT, 0xf4);

	global_char_input_driver = (char_input_driver_t*) driver;
}



char ps2_keyboard_async_getc(char_input_driver_t* driver) {
	char c = ((char*) driver->driver.driver_specific_data)[KBD_CHAR];
	((char*) driver->driver.driver_specific_data)[KBD_CHAR] = 0;

	return c;
}

char_input_driver_t* get_ps2_driver() {
	char_input_driver_t* driver = (char_input_driver_t*) vmm_alloc(1);
	memset(driver, 0, 4096);

	driver->driver.is_device_present = ps2_keyboard_is_device_present;
	driver->driver.get_device_name = ps2_keyboard_get_device_name;
	driver->driver.init = ps2_keyboard_init;
	
	driver->async_getc = ps2_keyboard_async_getc;

	driver->driver.driver_specific_data = driver + sizeof(char_input_driver_t);
	
	assert(is_arg((char*) global_multiboot_info->mbs_cmdline, "--keymap", &((char*) driver->driver.driver_specific_data)[KBD_KEYMAP_PATH]));

	return driver;
}