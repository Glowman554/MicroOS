#pragma once

#include <stdint.h>
#include <driver/driver.h>

enum video_mode_e {
	TEXT_80x25,
	CUSTOM
};

typedef struct char_output_driver {
	driver_t driver;
	void (*putc)(struct char_output_driver*, char);

	// optional
	int (*vmode)(struct char_output_driver*);
	void (*vpoke)(struct char_output_driver*, uint32_t offset, uint8_t* val, uint32_t range);
	void (*vpeek)(struct char_output_driver*, uint32_t offset, uint8_t* val, uint32_t range);

	void (*vcursor)(struct char_output_driver*, int x, int y);
	void (*vcursor_get)(struct char_output_driver*, int* x, int* y);

	void (*set_color)(struct char_output_driver*, char* color, bool background);
} char_output_driver_t;

extern char_output_driver_t* global_char_output_driver;