#include <driver/timer/pit.h>

#include <interrupts/interrupts.h>
#include <scheduler/scheduler.h>
#include <utils/io.h>
#include <stdio.h>

#define PIT_DIVISOR 65535

bool pit_is_device_present(driver_t* driver) {
	return true;
}

char* pit_get_device_name(driver_t* driver) {
	return "pit";
}

cpu_registers_t* pit_interrupt_handler(cpu_registers_t* registers, void* data) {
	driver_t* driver = (driver_t*) data;
	driver->driver_specific_data = (void*) ((uint32_t) driver->driver_specific_data + 1);

	return schedule(registers, data);
}

void pit_set_divisor(uint16_t divisor) {
	outb(0x40, (uint8_t)(divisor & 0x00ff));
	outb(0x40, (uint8_t)(divisor & 0xff00) >> 8);
}


void pit_init(driver_t* driver) {
	register_interrupt_handler(0x20, pit_interrupt_handler, driver);

	pit_set_divisor(PIT_DIVISOR);

	global_timer_driver = (timer_driver_t*) driver;
}

void pit_sleep(timer_driver_t* driver, uint32_t ms) {
	uint32_t start = (uint32_t) driver->driver.driver_specific_data;
	uint32_t end = start + ms;
	while((uint32_t) driver->driver.driver_specific_data < end) {
		asm volatile("nop");
	}
}

timer_driver_t pit_driver = {
	.driver = {
		.is_device_present = pit_is_device_present,
		.get_device_name = pit_get_device_name,
		.init = pit_init,
		.driver_specific_data = (void*) 0
	},
	.sleep = pit_sleep
};