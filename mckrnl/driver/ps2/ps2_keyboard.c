#include <ps2_keyboard.h>

#include <utils/io.h>
#include <utils/argparser.h>
#include <utils/multiboot.h>
#include <interrupts/interrupts.h>
#include <string.h>
#include <memory/heap.h>
#include <stdio.h>


typedef struct ps2_driver_data {
	bool special_key_next;
	uint8_t special_code;
	special_keys_down_t special_keys_down;

	char character;
	char arrow;
} ps2_driver_data_t;

bool ps2_keyboard_is_device_present(driver_t* driver) {
	return true;
}

char* ps2_keyboard_get_device_name(driver_t* driver) {
	return "ps2_keyboard";
}

cpu_registers_t* ps2_keyboard_interrupt_handler(cpu_registers_t* registers, void* driver) {
	driver_t* ps2_keyboard = (driver_t*) driver;

	ps2_driver_data_t* data = (ps2_driver_data_t*) ps2_keyboard->driver_specific_data;

	uint8_t key = inb(DATA_PORT);

	if (data->special_key_next) {
		switch (data->special_code) {
			case 0xE0:
				switch (key) {
					case 0x38: //Right alt down
						data->special_keys_down.right_alt = true;
						break;
					case 0xB8: //Right alt up
						data->special_keys_down.right_alt = false;
						break;
					case 0x1D: //Right ctrl down
						data->special_keys_down.right_ctrl = true;
						break;
					case 0x9D: //Right ctrl up
						data->special_keys_down.right_ctrl = false;
						break;
					case 0x48: //Up arrow down
						data->arrow = up;
						data->special_keys_down.up_arrow = true;
						break;
					case 0xC8: //Up arrow up
						data->special_keys_down.up_arrow = false;
						break;
					case 0x50: //Down arrow down
						data->arrow = down;
						data->special_keys_down.down_arrow = true;
						break;
					case 0xD0: //Down arrow up
						data->special_keys_down.down_arrow = false;
						break;
					case 0x4B: //Left arrow down
						data->arrow = left;
						data->special_keys_down.left_arrow = true;
						break;
					case 0xCB: //Left arrow down
						data->special_keys_down.left_arrow = false;
						break;
					case 0x4D: //Right arrow down
						data->arrow = right;
						data->special_keys_down.right_arrow = true;
						break;
					case 0xCD: //Right arrow up
						data->special_keys_down.right_arrow = false;
						break;
				}
				break;

			case 0x3A:
				switch (key) {
					case 0xBA: //Caps lock toggle
						data->special_keys_down.caps_lock = !data->special_keys_down.caps_lock;
						break;
				}
				break;
		}

		data->special_key_next = false;
	} else {
		switch (key) {
			case 0xE0:
			case 0x3A:
				data->special_key_next = true;
				data->special_code = key;
				break;
			
			case 0x38: //Left alt down
				data->special_keys_down.left_alt = true;
				break;
			case 0xB8: //Left alt up
				data->special_keys_down.left_alt = false;
				break;
			case 0x1D: //Left ctrl down
				data->special_keys_down.left_ctrl = true;
				break;
			case 0x9D: //Left ctrl up
				data->special_keys_down.left_ctrl = false;
				break;
			case 0x2A: //Left shift down
				data->special_keys_down.left_shift = true;
				break;
			case 0xAA: //Left shift up
				data->special_keys_down.left_shift = false;
				break;
			case 0x36: //Right shift down
				data->special_keys_down.right_shift = true;
				break; 
			case 0xB6: //Right shift up
				data->special_keys_down.right_shift = false;
				break;

			
			default:
				{
					if (key >= 0x3b && key <= 0x44) {
						global_fkey_handler(key - 0x3a, false);
					}
					if (key >= 0x57 && key <= 0x58) {
						global_fkey_handler(key - 0x4c, false);
					}

					if (key >= 0xbb && key <= 0xc4) {
						global_fkey_handler(key - 0xba, true);
					}
					if (key >= 0xd7 && key <= 0xd8) {
						global_fkey_handler(key - 0xcc, true);
					}



					char c = keymap(key, &data->special_keys_down);
					if ((data->special_keys_down.left_ctrl || data->special_keys_down.right_ctrl) && c == 'd') {
						c = -1; // EOF_CHAR
					}

					data->character = c;
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
	ps2_driver_data_t* data = (ps2_driver_data_t*) driver->driver.driver_specific_data;

	char c = data->character;
	data->character = 0;

	return c;
}

char ps2_keyboard_async_getarrw(char_input_driver_t* driver) {
	ps2_driver_data_t* data = (ps2_driver_data_t*) driver->driver.driver_specific_data;

	char c = data->arrow;
	data->arrow = 0;

	return c;
}

char_input_driver_t* get_ps2_driver() {
	char_input_driver_t* driver = (char_input_driver_t*) kmalloc(sizeof(char_input_driver_t) + sizeof(ps2_driver_data_t));
	memset(driver, 0, sizeof(char_input_driver_t) + sizeof(ps2_driver_data_t));

	driver->driver.is_device_present = ps2_keyboard_is_device_present;
	driver->driver.get_device_name = ps2_keyboard_get_device_name;
	driver->driver.init = ps2_keyboard_init;
	
	driver->async_getc = ps2_keyboard_async_getc;
	driver->async_getarrw = ps2_keyboard_async_getarrw;

	driver->driver.driver_specific_data = &driver[1];
	return driver;
}