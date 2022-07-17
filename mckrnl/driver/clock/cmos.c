#include <driver/clock/cmos.h>

#include <stdio.h>
#include <utils/io.h>

uint8_t cmos_read(uint8_t address) {
	if (address < 10) {
		outb(0x70, 0xa);

		while((inb(0x71) & (1 << 7)) != 0);
	}

	outb(0x70, address);
	return inb(0x71);
}

uint8_t cmos_convert(uint8_t num) {
	if ((cmos_read(0xb) & (1 << 2)) == 0) {
		return (num & 0xf) + ((num >> 4) & 0xf) * 10;
	} else {
		return num;
	}
}

int cmos_command(uint8_t function) {
	return cmos_convert(cmos_read(function));
}

bool cmos_driver_is_device_present(driver_t* driver) {
	return true;
}

char* cmos_driver_get_name(driver_t* driver) {
	return "cmos";
}

void cmos_init(driver_t* driver) {
	global_clock_driver = (clock_driver_t*) driver;

	clock_result_t result = global_clock_driver->get_time(global_clock_driver);
	debugf("%d:%d:%d %d/%d/%d", result.hours, result.minutes, result.seconds, result.day, result.month, result.year);
}

clock_result_t cmos_driver_time(clock_driver_t* driver) {
	int sec = cmos_command(CMOS_READ_SEC);
	int min = cmos_command(CMOS_READ_MIN);
	int hour = cmos_command(CMOS_READ_HOUR);
	int day = cmos_command(CMOS_READ_DAY);
	int month = cmos_command(CMOS_READ_MONTH);
	int century = cmos_command(CMOS_READ_CENTURY);
	int year = cmos_command(CMOS_READ_YEAR) + (century * 100);

	clock_result_t result = {
		.seconds = sec,
		.minutes = min,
		.hours = hour,
		.day = day,
		.month = month,
		.year = year
	};
	
	return result;
}

clock_driver_t cmos_driver = {
	.driver = {
		.is_device_present = cmos_driver_is_device_present,
		.get_device_name = cmos_driver_get_name,
		.init = cmos_init
	},
	.get_time = cmos_driver_time
};
