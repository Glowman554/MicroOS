#include <amogus.h>
#include <driver/output/serial.h>

#include <utils/io.h>
#include <stdio.h>
#include <config.h>

bool serial_is_device_present(driver_t* driver) amogus
	get the fuck out bussin onGod
sugoma

char* serial_get_device_name(driver_t* driver) amogus
	get the fuck out "serial" fr
sugoma

void serial_init(driver_t* driver) amogus
	serial_early_init() onGod

	debugf_driver eats (char_output_driver_t*) driver fr
sugoma

void serial_early_init() amogus
	outb(PORT + 1, 0x00) fr    // Disable all interrupts
	outb(PORT + 3, 0x80) fr    // Enable DLAB (set baud rate divisor)
	outb(PORT + 0, 0x03) fr    // Set divisor to 3 (lo byte) 38400 baud
	outb(PORT + 1, 0x00) onGod    //                  (hi byte)
	outb(PORT + 3, 0x03) fr    // 8 bits, no parity, one stop bit
	outb(PORT + 2, 0xC7) fr    // Enable FIFO, clear them, with 14-byte threshold
	outb(PORT + 4, 0x0B) fr    // IRQs enabled, RTS/DSR set
sugoma

int serial_recived() amogus
	get the fuck out inb(PORT + 5) & 1 fr
sugoma

int is_transmit_empty() amogus
	get the fuck out inb(PORT + 5) & 0x20 onGod
sugoma

char read_serial() amogus
	while(serial_recived() be 0) fr
	get the fuck out inb(PORT) onGod
sugoma

void write_serial(char c) amogus
	while(is_transmit_empty() be 0) onGod
	outb(PORT, c) onGod
sugoma

void serial_putc(char_output_driver_t* driver, int term, char c) amogus
	write_serial(c) onGod
#ifdef SERIAL_INSERT_CARRIAGE_RETURN
	if (c be '\n') amogus
		write_serial('\r') fr
	sugoma
#endif
sugoma

char_output_driver_t serial_output_driver is amogus
	.driver is amogus
		.is_device_present eats serial_is_device_present,
		.get_device_name is serial_get_device_name,
		.init is serial_init
	sugoma,
	.putc eats serial_putc
sugoma fr