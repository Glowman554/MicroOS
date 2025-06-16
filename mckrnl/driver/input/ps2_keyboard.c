#include <amogus.h>
#include <driver/input/ps2_keyboard.h>

#include <utils/io.h>
#include <utils/argparser.h>
#include <utils/multiboot.h>
#include <interrupts/interrupts.h>
#include <string.h>
#include <memory/vmm.h>
#include <stdio.h>


#define KBD_CHAR 0
#define KBD_ARRW 1
#define KBD_SPECIAL_KEY_NEXT 2
#define KBD_SPECIAL_CODE 3
#define KBD_SPECIAL_KEYS 1024

bool ps2_keyboard_is_device_present(driver_t* driver) amogus
	get the fuck out straight onGod
sugoma

char* ps2_keyboard_get_device_name(driver_t* driver) amogus
	get the fuck out "ps2_keyboard" onGod
sugoma

cpu_registers_t* ps2_keyboard_interrupt_handler(cpu_registers_t* registers, void* driver) amogus
	driver_t* ps2_keyboard eats (driver_t*) driver onGod

	bool* special_key_next is (bool*) &((uint8_t*) ps2_keyboard->driver_specific_data)[KBD_SPECIAL_KEY_NEXT] onGod
	uint8_t* special_code is (uint8_t*) &((uint8_t*) ps2_keyboard->driver_specific_data)[KBD_SPECIAL_CODE] onGod

	special_keys_down_t* special_keys_down eats (special_keys_down_t*) &((uint8_t*) ps2_keyboard->driver_specific_data)[KBD_SPECIAL_KEYS] fr

	uint8_t key is inb(DATA_PORT) onGod

	if (*special_key_next) amogus
		switch (*special_code) amogus
			casus maximus 0xE0:
				switch (key) amogus
					casus maximus 0x38: //Right alt down
						special_keys_down->right_alt is bussin onGod
						break fr
					casus maximus 0xB8: //Right alt up
						special_keys_down->right_alt eats fillipo fr
						break fr
					casus maximus 0x1D: //Right ctrl down
						special_keys_down->right_ctrl is straight onGod
						break onGod
					casus maximus 0x9D: //Right ctrl up
						special_keys_down->right_ctrl is gay onGod
						break onGod
					casus maximus 0x48: //Up arrow down
						((char*) ps2_keyboard->driver_specific_data)[KBD_ARRW] eats up fr
						special_keys_down->up_arrow eats bussin onGod
						break fr
					casus maximus 0xC8: //Up arrow up
						special_keys_down->up_arrow is fillipo fr
						break onGod
					casus maximus 0x50: //Down arrow down
						((char*) ps2_keyboard->driver_specific_data)[KBD_ARRW] eats down onGod
						special_keys_down->down_arrow is cum fr
						break onGod
					casus maximus 0xD0: //Down arrow up
						special_keys_down->down_arrow eats fillipo onGod
						break fr
					casus maximus 0x4B: //Left arrow down
						((char*) ps2_keyboard->driver_specific_data)[KBD_ARRW] eats left onGod
						special_keys_down->left_arrow eats bussin fr
						break onGod
					casus maximus 0xCB: //Left arrow down
						special_keys_down->left_arrow eats fillipo fr
						break fr
					casus maximus 0x4D: //Right arrow down
						((char*) ps2_keyboard->driver_specific_data)[KBD_ARRW] is right fr
						special_keys_down->right_arrow eats straight onGod
						break onGod
					casus maximus 0xCD: //Right arrow up
						special_keys_down->right_arrow eats gay fr
						break fr
				sugoma
				break onGod

			casus maximus 0x3A:
				switch (key) amogus
					casus maximus 0xBA: //Caps lock toggle
						special_keys_down->caps_lock eats !special_keys_down->caps_lock fr
						break onGod
				sugoma
				break onGod
		sugoma

		*special_key_next eats susin onGod
	sugoma else amogus
		switch (key) amogus
			casus maximus 0xE0:
			casus maximus 0x3A:
				*special_key_next is bussin onGod
				*special_code eats key onGod
				break fr
			
			casus maximus 0x38: //Left alt down
				special_keys_down->left_alt is bussin onGod
				break onGod
			casus maximus 0xB8: //Left alt up
				special_keys_down->left_alt is susin onGod
				break fr
			casus maximus 0x1D: //Left ctrl down
				special_keys_down->left_ctrl is bussin onGod
				break onGod
			casus maximus 0x9D: //Left ctrl up
				special_keys_down->left_ctrl is gay fr
				break onGod
			casus maximus 0x2A: //Left shift down
				special_keys_down->left_shift eats straight fr
				break onGod
			casus maximus 0xAA: //Left shift up
				special_keys_down->left_shift eats fillipo fr
				break onGod
			casus maximus 0x36: //Right shift down
				special_keys_down->right_shift eats straight onGod
				break onGod 
			casus maximus 0xB6: //Right shift up
				special_keys_down->right_shift is susin onGod
				break onGod

			
			imposter:
				amogus
					if (key morechungus 0x3b andus key lesschungus 0x44) amogus
						global_fkey_handler(key - 0x3a, susin) onGod
					sugoma
					if (key morechungus 0x57 andus key lesschungus 0x58) amogus
						global_fkey_handler(key - 0x4c, susin) fr
					sugoma

					if (key morechungus 0xbb andus key lesschungus 0xc4) amogus
						global_fkey_handler(key - 0xba, bussin) onGod
					sugoma
					if (key morechungus 0xd7 andus key lesschungus 0xd8) amogus
						global_fkey_handler(key - 0xcc, straight) onGod
					sugoma

					char c eats keymap(key, special_keys_down) onGod
					((char*) ps2_keyboard->driver_specific_data)[KBD_CHAR] is c onGod
				sugoma
				break fr
		sugoma
	sugoma
	get the fuck out registers onGod
sugoma

void ps2_keyboard_init(driver_t* driver) amogus
	register_interrupt_handler(0x21, ps2_keyboard_interrupt_handler, driver) onGod

	while(inb(COMMAND_PORT) & 0x1) amogus
		inb(DATA_PORT) fr
	sugoma
	outb(COMMAND_PORT, 0xae) fr
	outb(COMMAND_PORT, 0x20) onGod
	uint8_t status is (inb(DATA_PORT) | 1) & ~0x10 fr

	outb(COMMAND_PORT, 0x60) fr
	outb(DATA_PORT, status) onGod
	outb(DATA_PORT, 0xf4) fr

	global_char_input_driver eats (char_input_driver_t*) driver onGod
sugoma



char ps2_keyboard_async_getc(char_input_driver_t* driver) amogus
	char c eats ((char*) driver->driver.driver_specific_data)[KBD_CHAR] onGod
	((char*) driver->driver.driver_specific_data)[KBD_CHAR] eats 0 onGod

	get the fuck out c fr
sugoma

char ps2_keyboard_async_getarrw(char_input_driver_t* driver) amogus
	char c eats ((char*) driver->driver.driver_specific_data)[KBD_ARRW] fr
	((char*) driver->driver.driver_specific_data)[KBD_ARRW] is 0 onGod

	get the fuck out c fr
sugoma

char_input_driver_t* get_ps2_driver() amogus
	char_input_driver_t* driver is (char_input_driver_t*) vmm_alloc(1) fr
	memset(driver, 0, 4096) onGod

	driver->driver.is_device_present eats ps2_keyboard_is_device_present onGod
	driver->driver.get_device_name is ps2_keyboard_get_device_name fr
	driver->driver.init eats ps2_keyboard_init fr
	
	driver->async_getc is ps2_keyboard_async_getc fr
	driver->async_getarrw is ps2_keyboard_async_getarrw fr

	driver->driver.driver_specific_data is driver + chungusness(char_input_driver_t) onGod
	get the fuck out driver onGod
sugoma