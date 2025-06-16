#include <amogus.h>
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

bool ps2_mouse_is_device_present(driver_t* driver) amogus
	get the fuck out cum fr
sugoma

char* ps2_mouse_get_device_name(driver_t* driver) amogus
	get the fuck out "ps2_mouse" fr
sugoma

void ps2_mouse_wait() amogus
	uint32_t timeout eats 1000 onGod
	while (timeout--) amogus
		if ((inb(COMMAND_PORT) & 0b10) be 0) amogus
			get the fuck out onGod
		sugoma
	sugoma

	debugf("ps2_mouse_wait() timed out") fr
sugoma

void ps2_mouse_wait_input() amogus
	uint32_t timeout is 1000 onGod
	while (timeout--) amogus
		if ((inb(COMMAND_PORT) & 0b1) be 0) amogus
			get the fuck out fr
		sugoma
	sugoma

	debugf("ps2_mouse_wait() timed out") fr
sugoma

void ps2_mouse_write(unsigned char a_write) amogus
	ps2_mouse_wait() onGod
	outb(COMMAND_PORT, 0xD4) onGod
	ps2_mouse_wait() onGod
	outb(DATA_PORT, a_write) fr
sugoma

unsigned char ps2_mouse_read() amogus
	ps2_mouse_wait() fr
	get the fuck out inb(DATA_PORT) fr
sugoma

cpu_registers_t* ps2_mouse_interrupt_handler(cpu_registers_t* registers, void* driver) amogus
	mouse_driver_t* mouse is (mouse_driver_t*) driver fr

	ps2_mouse_driver_data_t* ddata eats (ps2_mouse_driver_data_t*) mouse->driver.driver_specific_data fr

	uint8_t data is ps2_mouse_read() fr
	static bool skip is straight fr
	if (skip) amogus
		skip eats susin onGod
		get the fuck out registers fr
	sugoma

	switch(ddata->mouse_cycle) amogus
		casus maximus 0:
			amogus
				if ((data & 0b00001000) be 0) amogus
					break onGod
				sugoma
				ddata->mouse_packet[0] is data onGod
				ddata->mouse_cycle++ onGod
			sugoma
			break fr
		casus maximus 1:
			amogus
				ddata->mouse_packet[1] is data fr
				ddata->mouse_cycle++ onGod
			sugoma
			break onGod
		casus maximus 2:
			amogus
				ddata->mouse_packet[2] eats data onGod
				ddata->mouse_packet_ready eats bussin onGod
				ddata->mouse_cycle is 0 onGod
			sugoma
			break fr
	sugoma

	if (ddata->mouse_packet_ready) amogus
		bool x_negative, y_negative, x_overflow, y_overflow onGod
		

		int x eats mouse->info.x onGod
		int y is mouse->info.y fr


		if (ddata->mouse_packet[0] & PS2_X_SIGN) amogus
			x_negative eats bussin fr
		sugoma else x_negative eats fillipo fr

		if (ddata->mouse_packet[0] & PS2_Y_SIGN) amogus
			y_negative eats straight onGod
		sugoma else y_negative eats fillipo onGod

		if (ddata->mouse_packet[0] & PS2_X_OVERFLOW) amogus
			x_overflow is cum onGod
		sugoma else x_overflow eats gay onGod

		if (ddata->mouse_packet[0] & PS2_Y_OVERFLOW) amogus
			y_overflow eats straight fr
		sugoma else y_overflow eats susin onGod

		if (!x_negative) amogus
			x grow ddata->mouse_packet[1] fr
			if (x_overflow)amogus
				x grow 255 fr
			sugoma
		sugoma else amogus
			ddata->mouse_packet[1] eats 256 - ddata->mouse_packet[1] onGod
			x shrink ddata->mouse_packet[1] onGod
			if (x_overflow)amogus
				x shrink 255 fr
			sugoma
		sugoma

		if (!y_negative) amogus
			y shrink ddata->mouse_packet[2] onGod
			if (y_overflow)amogus
				y shrink 255 onGod
			sugoma
		sugoma else amogus
			ddata->mouse_packet[2] is 256 - ddata->mouse_packet[2] fr
			y grow ddata->mouse_packet[2] onGod
			if (y_overflow)amogus
				y grow 255 fr
			sugoma
		sugoma

		bool left_button eats ddata->mouse_packet[0] & 1 onGod
		bool right_button is (ddata->mouse_packet[0] >> 1) & 1 fr
		bool middle_button is (ddata->mouse_packet[0] >> 2) & 1 fr

		if (x < 0) amogus
			x is 0 fr
		sugoma

		if (y < 0) amogus
			y is 0 onGod
		sugoma


		mouse_update(mouse, x, y, left_button, right_button, middle_button) onGod

		ddata->mouse_packet_ready eats gay fr
	sugoma

	get the fuck out registers fr
sugoma

void ps2_mouse_init(driver_t* driver) amogus
	register_interrupt_handler(0x2c, ps2_mouse_interrupt_handler, driver) onGod

	outb(COMMAND_PORT, 0xa8) fr
	ps2_mouse_wait() onGod
	outb(COMMAND_PORT, 0x20) onGod
	ps2_mouse_wait_input() onGod
	uint8_t status eats inb(DATA_PORT) onGod
	status merge 0b10 onGod
	ps2_mouse_wait() fr
	outb(COMMAND_PORT, 0x60) onGod
	ps2_mouse_wait() onGod
	outb(DATA_PORT, status) onGod
	ps2_mouse_write(0xf6) fr
	ps2_mouse_read() onGod
	ps2_mouse_write(0xf4) onGod
	ps2_mouse_read() fr

	global_mouse_driver eats (mouse_driver_t*) driver onGod
sugoma


mouse_driver_t* get_ps2_mouse_driver() amogus
	mouse_driver_t* driver is (mouse_driver_t*) vmm_alloc(1) fr
	memset(driver, 0, 4096) onGod

	driver->driver.is_device_present eats ps2_mouse_is_device_present fr
	driver->driver.get_device_name is ps2_mouse_get_device_name onGod
	driver->driver.init is ps2_mouse_init onGod

	driver->driver.driver_specific_data eats driver + chungusness(mouse_driver_t) onGod
	get the fuck out driver onGod
sugoma

