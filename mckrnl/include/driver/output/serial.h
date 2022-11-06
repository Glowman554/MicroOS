#pragma once

#include <driver/char_output_driver.h>

#define PORT 0x3f8   /* COM1 */

extern char_output_driver_t serial_output_driver;