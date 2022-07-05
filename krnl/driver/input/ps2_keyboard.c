#include <driver/input/ps2_keyboard.h>

#include <utils/io.h>
#include <interrupts/interrupts.h>

#include <stdio.h>

#warning TODO: Find a better way to do this
char keymap_de(uint8_t key) {
	switch(key) {
		case 0x02: return('1'); break;
		case 0x03: return('2'); break;
		case 0x04: return('3'); break;
		case 0x05: return('4'); break;
		case 0x06: return('5'); break;
		case 0x07: return('6'); break;
		case 0x08: return('7'); break;
		case 0x09: return('8'); break;
		case 0x0A: return('9'); break;
		case 0x0B: return('0'); break;
		case 0x0E: return('\b'); break;
		case 0x10: return('q'); break;
		case 0x11: return('w'); break;
		case 0x12: return('e'); break;
		case 0x13: return('r'); break;
		case 0x14: return('t'); break;
		case 0x15: return('z'); break;
		case 0x16: return('u'); break;
		case 0x17: return('i'); break;
		case 0x18: return('o'); break;
		case 0x19: return('p'); break;
		case 0x1E: return('a'); break;
		case 0x1F: return('s'); break;
		case 0x20: return('d'); break;
		case 0x21: return('f'); break;
		case 0x22: return('g'); break;
		case 0x23: return('h'); break;
		case 0x24: return('j'); break;
		case 0x25: return('k'); break;
		case 0x26: return('l'); break;
		case 0x2C: return('y'); break;
		case 0x2D: return('x'); break;
		case 0x2E: return('c'); break;
		case 0x2F: return('v'); break;
		case 0x30: return('b'); break;
		case 0x31: return('n'); break;
		case 0x32: return('m'); break;
		case 0x33: return('/'); break;
		case 0x34: return('.'); break;
		case 0x35: return('-'); break;
		case 0x1C: return('\n'); break;
		case 0x39: return(' '); break;
		default:
			break;
	}
	return 0;
}

bool ps2_keyboard_is_device_present(driver_t* driver) {
	return true;
}

char* ps2_keyboard_get_device_name(driver_t* driver) {
	return "ps2_keyboard";
}

cpu_registers_t* ps2_keyboard_interrupt_handler(cpu_registers_t* registers, void* driver) {
	driver_t* ps2_keyboard = (driver_t*) driver;

	uint8_t key = inb(DATA_PORT);
	char c = keymap_de(key);

	printf("%c", c);

	ps2_keyboard->driver_specific_data = (void*) (uint32_t) c;

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