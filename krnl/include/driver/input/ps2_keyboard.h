#pragma once

#include <driver/char_input_driver.h>

#define DATA_PORT 0x60
#define COMMAND_PORT 0x64

extern char_input_driver_t ps2_keyboard_driver;