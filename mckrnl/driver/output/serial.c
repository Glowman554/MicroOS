#include <driver/output/serial.h>

#include <utils/io.h>
#include <stdio.h>

bool serial_is_device_present(driver_t* driver) {
	return true;
}

char* serial_get_device_name(driver_t* driver) {
	return "serial";
}

void serial_init(driver_t* driver) {
	serial_early_init();

	debugf_driver = (char_output_driver_t*) driver;
}

void serial_early_init() {
	outb(PORT + 1, 0x00);    // Disable all interrupts
	outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	outb(PORT + 1, 0x00);    //                  (hi byte)
	outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
	outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int serial_recived() {
	return inb(PORT + 5) & 1;
}

int is_transmit_empty() {
	return inb(PORT + 5) & 0x20;
}

char read_serial() {
	while(serial_recived() == 0);
	return inb(PORT);
}

void write_serial(char c) {
	while(is_transmit_empty() == 0);
	outb(PORT, c);
}

void serial_putc(char_output_driver_t* driver, int term, char c) {
	write_serial(c);
}

char_output_driver_t serial_output_driver = {
	.driver = {
		.is_device_present = serial_is_device_present,
		.get_device_name = serial_get_device_name,
		.init = serial_init
	},
	.putc = serial_putc
};