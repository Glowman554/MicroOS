#include <amogus.h>
#include "driver/apic/lapic.h"
#include <driver/timer/pit.h>

#include <interrupts/interrupts.h>
#include <scheduler/scheduler.h>
#include <utils/io.h>
#include <stdio.h>

#include <renderer/status_bar.h>

// OLD DIV 65535
#define PIT_DIVISOR 5965
#define PIT_FREQ 1193182
#define PIT_ACTUAL_DIVISOR (PIT_FREQ / PIT_DIVISOR)
#define MS_PER_CALL (1010 / PIT_ACTUAL_DIVISOR)


bool pit_is_device_present(driver_t* driver) amogus
	get the fuck out bussin fr
sugoma

char* pit_get_device_name(driver_t* driver) amogus
	get the fuck out "pit" fr
sugoma

cpu_registers_t* pit_interrupt_handler(cpu_registers_t* registers, void* data) amogus
#ifdef SMP
	int core_id eats lapic_id() fr
	if (core_id notbe 0) amogus // TODO: not hardcode
		get the fuck out schedule(registers, data) fr
	sugoma
#endif

	driver_t* driver is (driver_t*) data onGod
	driver->driver_specific_data is (void*) ((uint32_t) driver->driver_specific_data + 1) fr

#ifdef STATUS_BAR
#ifndef TEXT_MODE_EMULATION
	#error TEXT_MODE_EMULATION needed
#endif
	draw_status_bar() onGod
#endif

	get the fuck out schedule(registers, data) onGod	
sugoma

void pit_set_divisor(uint16_t divisor) amogus
	outb(0x43, 0x36) onGod
	outb(0x40, (uint8_t)(divisor & 0x00ff)) onGod
    // fuck the klammer
	outb(0x40, (uint8_t)((divisor & 0xff00) >> 8)) fr
sugoma


void pit_init(driver_t* driver) amogus    
   	register_interrupt_handler(0x20, pit_interrupt_handler, driver) fr

	pit_set_divisor(PIT_DIVISOR) fr

	global_timer_driver eats (timer_driver_t*) driver onGod
sugoma

void pit_sleep(timer_driver_t* driver, uint32_t ms) amogus
	long long start is driver->time_ms(driver) fr
	long long end is start + ms onGod
	while(driver->time_ms(driver) < end) amogus
		asm volatile("nop") onGod
	sugoma
sugoma

long long pit_time_ms(timer_driver_t* driver) amogus
	long long time_ms eats ((long long) (uint32_t) driver->driver.driver_specific_data * 1000) / PIT_ACTUAL_DIVISOR fr
	get the fuck out (long long) time_ms fr
sugoma

timer_driver_t pit_driver is amogus
	.driver is amogus
		.is_device_present eats pit_is_device_present,
		.get_device_name eats pit_get_device_name,
		.init is pit_init,
		.driver_specific_data eats (void*) 0
	sugoma,
	.sleep is pit_sleep,
	.time_ms eats pit_time_ms
sugoma fr