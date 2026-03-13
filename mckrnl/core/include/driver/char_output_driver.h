#pragma once

#include "config.h"
#include <stdint.h>
#include <stdbool.h>
#include <driver/driver.h>

enum video_mode_e {
	TEXT_80x25,
	CUSTOM
};

enum ansi_erase_mode {
	ERASE_CURSOR_TO_END = 0,
	ERASE_BEGINNING_TO_CURSOR = 1,
	ERASE_ENTIRE = 2,
	ERASE_SAVED_LINES = 3
};

typedef struct char_output_driver {
	driver_t driver;
	void (*putc)(struct char_output_driver*, int term, char);

	// optional
	int (*vmode)(struct char_output_driver*);
	void (*vpoke)(struct char_output_driver*, int term, uint32_t offset, uint8_t* val, uint32_t range);
	void (*vpeek)(struct char_output_driver*, int term, uint32_t offset, uint8_t* val, uint32_t range);
	void (*vterm)(struct char_output_driver*, int term);

	void (*vcursor)(struct char_output_driver*, int term, int x, int y);
	void (*vcursor_get)(struct char_output_driver*, int term, int* x, int* y);

	void (*set_color)(struct char_output_driver*, int term, char* color, bool background);
	void (*rgb_color)(struct char_output_driver*, int term, uint32_t color, bool background);
	
	void (*erase_display)(struct char_output_driver*, int term, enum ansi_erase_mode mode);
	void (*erase_line)(struct char_output_driver*, int term, enum ansi_erase_mode mode);

	int current_term;

	bool ansi_esc[MAX_VTERM];
	char ansi_esc_buffer[MAX_VTERM][32];
	int ansi_esc_buffer_index[MAX_VTERM];
	bool ansi_bright[MAX_VTERM];
} char_output_driver_t;

extern char_output_driver_t* global_char_output_driver;