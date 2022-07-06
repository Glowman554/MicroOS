#pragma once

#include <driver/driver.h>
#include <stdbool.h>

typedef struct char_input_driver {
	driver_t driver;
	char (*async_getc)(struct char_input_driver*);
} char_input_driver_t;

typedef struct special_keys_down_t {
	bool left_shift;
	bool left_ctrl;
	bool left_alt;
	bool left_gui;

	bool right_shift;
	bool right_ctrl;
	bool right_alt;
	bool right_gui;

	bool caps_lock;
	bool num_lock;
	bool scroll_lock;

	bool up_arrow;
	bool down_arrow;
	bool left_arrow;
	bool right_arrow;
} special_keys_down_t;

typedef struct keymap_t {
	char layout_normal[0xff];
	char layout_shift[0xff];
	char layout_alt[0xff];
} keymap_t;

extern char_input_driver_t* global_char_input_driver;

char keymap(char* keymap_path, uint8_t key, special_keys_down_t* special_keys_down);