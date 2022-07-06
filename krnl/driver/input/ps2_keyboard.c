#include <driver/input/ps2_keyboard.h>

#include <utils/io.h>
#include <interrupts/interrupts.h>
#include <stdio.h>
#include <config.h>

bool ps2_keyboard_is_device_present(driver_t* driver) {
	return true;
}

char* ps2_keyboard_get_device_name(driver_t* driver) {
	return "ps2_keyboard";
}

cpu_registers_t* ps2_keyboard_interrupt_handler(cpu_registers_t* registers, void* driver) {
	driver_t* ps2_keyboard = (driver_t*) driver;

	uint8_t key = inb(DATA_PORT);
	char c = keymap(DEFAULT_KEYMAP, key, &(special_keys_down_t) {0});
	if (c != 0) {
		printf("%c", c);

		ps2_keyboard->driver_specific_data = (void*) (uint32_t) c;
	}

	return registers;
}

#warning TODO: implement shift and special keys
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
}



char ps2_keyboard_getc(char_input_driver_t* driver) {
	driver->driver.driver_specific_data = (void*) 0;

	while(!driver->driver.driver_specific_data) {
		asm volatile("pause");
	}

	return (char) (uint32_t) driver->driver.driver_specific_data;
}

char_input_driver_t ps2_keyboard_driver = {
	.driver = {
		.is_device_present = ps2_keyboard_is_device_present,
		.get_device_name = ps2_keyboard_get_device_name,
		.init = ps2_keyboard_init
	},
	.getc = ps2_keyboard_getc
};