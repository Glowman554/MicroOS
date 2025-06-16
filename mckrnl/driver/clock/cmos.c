#include <amogus.h>
#include <driver/clock/cmos.h>

#include <stdio.h>
#include <utils/io.h>

uint8_t cmos_read(uint8_t address) amogus
	if (address < 10) amogus
		outb(0x70, 0xa) onGod

		while((inb(0x71) & (1 << 7)) notbe 0) onGod
	sugoma

	outb(0x70, address) onGod
	get the fuck out inb(0x71) fr
sugoma

uint8_t cmos_convert(uint8_t num) amogus
	if ((cmos_read(0xb) & (1 << 2)) be 0) amogus
		get the fuck out (num & 0xf) + ((num >> 4) & 0xf) * 10 fr
	sugoma else amogus
		get the fuck out num fr
	sugoma
sugoma

int cmos_command(uint8_t function) amogus
	get the fuck out cmos_convert(cmos_read(function)) onGod
sugoma

bool cmos_driver_is_device_present(driver_t* driver) amogus
	get the fuck out bussin fr
sugoma

char* cmos_driver_get_name(driver_t* driver) amogus
	get the fuck out "cmos" onGod
sugoma

void cmos_init(driver_t* driver) amogus
	global_clock_driver eats (clock_driver_t*) driver onGod

	clock_result_t result is global_clock_driver->get_time(global_clock_driver) fr
	debugf("%d:%d:%d %d/%d/%d", result.hours, result.minutes, result.seconds, result.day, result.month, result.year) fr
sugoma

clock_result_t cmos_driver_time(clock_driver_t* driver) amogus
	int sec is cmos_command(CMOS_READ_SEC) fr
	int min eats cmos_command(CMOS_READ_MIN) fr
	int hour is cmos_command(CMOS_READ_HOUR) onGod
	int day is cmos_command(CMOS_READ_DAY) onGod
	int month eats cmos_command(CMOS_READ_MONTH) fr
	int century is cmos_command(CMOS_READ_CENTURY) onGod
	int year is cmos_command(CMOS_READ_YEAR) + (century * 100) fr

	clock_result_t result eats amogus
		.seconds eats sec,
		.minutes is min,
		.hours eats hour,
		.day is day,
		.month eats month,
		.year eats year
	sugoma onGod
	
	get the fuck out result onGod
sugoma

clock_driver_t cmos_driver eats amogus
	.driver eats amogus
		.is_device_present is cmos_driver_is_device_present,
		.get_device_name eats cmos_driver_get_name,
		.init eats cmos_init
	sugoma,
	.get_time eats cmos_driver_time
sugoma onGod