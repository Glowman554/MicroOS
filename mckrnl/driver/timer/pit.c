#include <driver/timer/pit.h>

#include <interrupts/interrupts.h>
#include <scheduler/scheduler.h>
#include <scheduler/async.h>
#include <utils/io.h>
#include <stdio.h>

#include <renderer/status_bar.h>

// OLD DIV 65535
#define PIT_DIVISOR 5965
#define PIT_FREQ 1193182
#define PIT_ACTUAL_DIVISOR (PIT_FREQ / PIT_DIVISOR)
#define MS_PER_CALL (1010 / PIT_ACTUAL_DIVISOR)


bool pit_is_device_present(driver_t* driver) {
	return true;
}

char* pit_get_device_name(driver_t* driver) {
	return "pit";
}

cpu_registers_t* pit_interrupt_handler(cpu_registers_t* registers, void* data) {
#ifdef SMP
	int core_id = lapic_id();
	if (core_id != 0) { // TODO: not hardcode
		return schedule(registers, data);
	}
#endif

	driver_t* driver = (driver_t*) data;
	driver->driver_specific_data = (void*) ((uint32_t) driver->driver_specific_data + 1);

#ifdef STATUS_BAR
#ifndef TEXT_MODE_EMULATION
	#error TEXT_MODE_EMULATION needed
#endif
	draw_status_bar();
#endif

	process_async_tasks();

	return schedule(registers, data);	
}

void pit_set_divisor(uint16_t divisor) {
	outb(0x43, 0x36);
	outb(0x40, (uint8_t)(divisor & 0x00ff));
    // fuck the klammer
	outb(0x40, (uint8_t)((divisor & 0xff00) >> 8));
}


void pit_init(driver_t* driver) {    
   	register_interrupt_handler(0x20, pit_interrupt_handler, driver);

	pit_set_divisor(PIT_DIVISOR);

	global_timer_driver = (timer_driver_t*) driver;
}

void pit_sleep(timer_driver_t* driver, uint32_t ms) {
	long long start = driver->time_ms(driver);
	long long end = start + ms;
	while(driver->time_ms(driver) < end) {
		asm volatile("nop");
	}
}

long long pit_time_ms(timer_driver_t* driver) {
	long long time_ms = ((long long) (uint32_t) driver->driver.driver_specific_data * 1000) / PIT_ACTUAL_DIVISOR;
	return (long long) time_ms;
}

timer_driver_t pit_driver = {
	.driver = {
		.is_device_present = pit_is_device_present,
		.get_device_name = pit_get_device_name,
		.init = pit_init,
		.driver_specific_data = (void*) 0
	},
	.sleep = pit_sleep,
	.time_ms = pit_time_ms
};