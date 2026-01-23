#pragma once

#include <driver/char_input_driver.h>

#define DATA_PORT 0x60
#define COMMAND_PORT 0x64

char_input_driver_t* get_ps2_driver();