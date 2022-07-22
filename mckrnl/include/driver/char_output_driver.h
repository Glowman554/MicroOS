#pragma once

#include <stdint.h>
#include <driver/driver.h>

enum video_mode_e {
	TEXT_80x25,
};

typedef struct char_output_driver {
	driver_t driver;
	void (*putc)(struct char_output_driver*, char);

	// optional
	int (*vmode)(struct char_output_driver*);
	void (*vpoke)(struct char_output_driver*, uint32_t offset, uint8_t value);
} char_output_driver_t;

extern char_output_driver_t* global_char_output_driver;