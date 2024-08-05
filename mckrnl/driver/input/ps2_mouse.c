#include <driver/input/ps2_mouse.h>

#include <utils/io.h>
#include <utils/argparser.h>
#include <utils/multiboot.h>
#include <interrupts/interrupts.h>
#include <stdio.h>
#include <string.h>
#include <memory/vmm.h>

#define PS2_X_SIGN 0b00010000
#define PS2_Y_SIGN 0b00100000
#define PS2_X_OVERFLOW 0b01000000
#define PS2_Y_OVERFLOW 0b10000000

#define DATA_PORT 0x60
#define COMMAND_PORT 0x64

bool ps2_mouse_is_device_present(driver_t* driver) {
	return true;
}

char* ps2_mouse_get_device_name(driver_t* driver) {
	return "ps2_mouse";
}

void ps2_mouse_wait() {
	uint32_t timeout = 1000;
	while (timeout--) {
		if ((inb(COMMAND_PORT) & 0b10) == 0) {
			return;
		}
	}

	debugf("ps2_mouse_wait() timed out");
}

void ps2_mouse_wait_input() {
	uint32_t timeout = 1000;
	while (timeout--) {
		if ((inb(COMMAND_PORT) & 0b1) == 0) {
			return;
		}
	}

	debugf("ps2_mouse_wait() timed out");
}

void ps2_mouse_write(unsigned char a_write) {
	ps2_mouse_wait();
	outb(COMMAND_PORT, 0xD4);
	ps2_mouse_wait();
	outb(DATA_PORT, a_write);
}

unsigned char ps2_mouse_read() {
	ps2_mouse_wait();
	return inb(DATA_PORT);
}

cpu_registers_t* ps2_mouse_interrupt_handler(cpu_registers_t* registers, void* driver) {
	mouse_driver_t* mouse = (mouse_driver_t*) driver;

	ps2_mouse_driver_data_t* ddata = (ps2_mouse_driver_data_t*) mouse->driver.driver_specific_data;

	uint8_t data = ps2_mouse_read();
	static bool skip = true;
	if (skip) {
		skip = false;
		return registers;
	}

	switch(ddata->mouse_cycle) {
		case 0:
			{
				if ((data & 0b00001000) == 0) {
					break;
				}
				ddata->mouse_packet[0] = data;
				ddata->mouse_cycle++;
			}
			break;
		case 1:
			{
				ddata->mouse_packet[1] = data;
				ddata->mouse_cycle++;
			}
			break;
		case 2:
			{
				ddata->mouse_packet[2] = data;
				ddata->mouse_packet_ready = true;
				ddata->mouse_cycle = 0;
			}
			break;
	}

	if (ddata->mouse_packet_ready) {
		bool x_negative, y_negative, x_overflow, y_overflow;
		

		int x = mouse->info.x;
		int y = mouse->info.y;


		if (ddata->mouse_packet[0] & PS2_X_SIGN) {
			x_negative = true;
		} else x_negative = false;

		if (ddata->mouse_packet[0] & PS2_Y_SIGN) {
			y_negative = true;
		} else y_negative = false;

		if (ddata->mouse_packet[0] & PS2_X_OVERFLOW) {
			x_overflow = true;
		} else x_overflow = false;

		if (ddata->mouse_packet[0] & PS2_Y_OVERFLOW) {
			y_overflow = true;
		} else y_overflow = false;

		if (!x_negative) {
			x += ddata->mouse_packet[1];
			if (x_overflow){
				x += 255;
			}
		} else {
			ddata->mouse_packet[1] = 256 - ddata->mouse_packet[1];
			x -= ddata->mouse_packet[1];
			if (x_overflow){
				x -= 255;
			}
		}

		if (!y_negative) {
			y -= ddata->mouse_packet[2];
			if (y_overflow){
				y -= 255;
			}
		} else {
			ddata->mouse_packet[2] = 256 - ddata->mouse_packet[2];
			y += ddata->mouse_packet[2];
			if (y_overflow){
				y += 255;
			}
		}

		bool left_button = ddata->mouse_packet[0] & 1;
		bool right_button = (ddata->mouse_packet[0] >> 1) & 1;
		bool middle_button = (ddata->mouse_packet[0] >> 2) & 1;

		if (x < 0) {
			x = 0;
		}

		if (y < 0) {
			y = 0;
		}


		mouse_update(mouse, x, y, left_button, right_button, middle_button);

		ddata->mouse_packet_ready = false;
	}

	return registers;
}

void ps2_mouse_init(driver_t* driver) {
	register_interrupt_handler(0x2c, ps2_mouse_interrupt_handler, driver);

	outb(COMMAND_PORT, 0xa8);
	ps2_mouse_wait();
	outb(COMMAND_PORT, 0x20);
	ps2_mouse_wait_input();
	uint8_t status = inb(DATA_PORT);
	status |= 0b10;
	ps2_mouse_wait();
	outb(COMMAND_PORT, 0x60);
	ps2_mouse_wait();
	outb(DATA_PORT, status);
	ps2_mouse_write(0xf6);
	ps2_mouse_read();
	ps2_mouse_write(0xf4);
	ps2_mouse_read();

	global_mouse_driver = (mouse_driver_t*) driver;
}


mouse_driver_t* get_ps2_mouse_driver() {
	mouse_driver_t* driver = (mouse_driver_t*) vmm_alloc(1);
	memset(driver, 0, 4096);

	driver->driver.is_device_present = ps2_mouse_is_device_present;
	driver->driver.get_device_name = ps2_mouse_get_device_name;
	driver->driver.init = ps2_mouse_init;

	driver->driver.driver_specific_data = driver + sizeof(mouse_driver_t);
	return driver;
}

